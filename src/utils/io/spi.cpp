/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 18. April 2018
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include "utils/io/spi.hpp"

// #include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef WIN
#include <linux/spi/spidev.h>
#else
#define _IOW(type, nr, size) 10   // random demo functionality
#define SPI_IOC_MAGIC             'k'
#define SPI_IOC_WR_MODE           _IOW(SPI_IOC_MAGIC, 1, uint8_t)
#define SPI_IOC_WR_MAX_SPEED_HZ   _IOW(SPI_IOC_MAGIC, 4, uint32_t)
#define SPI_IOC_WR_LSB_FIRST      _IOW(SPI_IOC_MAGIC, 2, uint8_t)
#define SPI_IOC_WR_BITS_PER_WORD  _IOW(SPI_IOC_MAGIC, 3, uint8_t)
struct spi_ioc_transfer {
  uint64_t tx_buf;
  uint64_t rx_buf;

  uint32_t len;
  uint32_t speed_hz;

  uint16_t delay_usecs;
  uint8_t  bits_per_word;
  uint8_t  cs_change;
  uint8_t  tx_nbits;
  uint8_t  rx_nbits;
  uint16_t pad;
};
#define SPI_MSGSIZE(N) \
  ((((N)*(sizeof(struct spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
    ? ((N)*(sizeof(struct spi_ioc_transfer))) : 0)
#define SPI_IOC_MESSAGE(N)  _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(N)])
#define SPI_CS_HIGH         0x04
#endif  // ifndef WIN

#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

// configure SPI
#define SPI_MODE  3
#define SPI_BITS  8         // each word is 1B
#define SPI_MSBFIRST 0
#define SPI_LSBFIRST 1


namespace hyped {
namespace utils {
namespace io {

constexpr const off_t bases[kBankNum] = {
  0x48030000,
  0x481A0000  
};

constexpr uint32_t kMmapSize = 0x1000;
void* base_mapping_[kBankNum];

// register offsets

constexpr uint32_t MCSPI_SYSCONFIG = 0x110;
constexpr uint32_t MCSPI_CH0CONF = 0x12C;
volatile uint32_t* data_;

bool initialised_ = false;

struct channel {
  volatile uint32_t config;
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t tx_buf;
  volatile uint32_t rx_buf;
};

channel* ch0;

SPI& SPI::getInstance()
{
  static SPI spi(System::getLogger());
  return spi;
}

SPI::SPI(Logger& log)
    : log_(log),
    transfer_init_(false)
{
  if (!initialised_) initialise();

  const char device[] = "/dev/spidev1.0";
  spi_fd_ = open(device, O_RDWR, 0);

  if (spi_fd_ < 0) {
    log_.ERR("SPI", "Could not open spi device");
    return;
  }

  // set clock frequency
  setClock(Clock::k1MHz);

  // set bits per word12Ch MCSPI_CH0CONF McSPI channel 0 configuration register Section 24.4.1.8
// 130h MCSPI_CH0STAT McSPI channel 0 status register Section 24.4.1.9
// 134h MCSPI_CH0CTRL McSPI channel 0 control register Section 24.4.1.10
// 138h MCSPI_TX0 McSPI channel 0 FIFO transmit buffer register Section 24.4.1.11
// 13Ch MCSPI_RX0 McSPI channel 0 FIFO receive
  uint8_t bits = SPI_BITS;      // need to change this value
  if (ioctl(spi_fd_, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
    log_.ERR("SPI", "could not set bits per word");
  }

  // set clock mode and CS active low
  uint8_t mode = (SPI_MODE & 0x3) & ~SPI_CS_HIGH;
  if (ioctl(spi_fd_, SPI_IOC_WR_MODE, &mode) < 0) {
    log_.ERR("SPI", "could not set mode");
  }

  // set bit order
  uint8_t order = SPI_MSBFIRST;
  if (ioctl(spi_fd_, SPI_IOC_WR_LSB_FIRST, &order) < 0) {
    log_.ERR("SPI", "could not set bit order");
  }

  log_.INFO("SPI", "spi instance created");
}

void SPI::initialise()
{
  int   fd;
  off_t offset;
  void* base;
  Logger log(false, -1);

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    log.ERR("SPI", "could not open /dev/mem");
    return;
  }

  for (int i = 0; i < kBankNum; i++) {
    offset = bases[i];
    base = mmap(0, kMmapSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                fd, offset);
    if (base == MAP_FAILED) {
      log.ERR("SPI", "could not map bank %d", offset);
      return;
    }

    base_mapping_[i] = base;
  }
//   struct channel {
//   volatile uint32_t* config;
//   volatile uint32_t* status;
//   volatile uint32_t* control;
//   volatile uint32_t* tx;
//   volatile uint32_t* rx;
// };
    // cho0 config
  // ch0->config = reinterpret_cast<volatile uint32_t*>(base_0 + 0x12C);
  // ch0->status = reinterpret_cast<volatile uint32_t*>(base_0 + 0x130);
  // ch0->control = reinterpret_cast<volatile uint32_t*>(base_0 + 0x134);
  // ch0->tx = reinterpret_cast<volatile uint32_t*>(base_0 + 0x138);
  // ch0->rx = reinterpret_cast<volatile uint32_t*>(base_0 + 0x13C);
  


  // atexit(uninitialise);

  initialised_ = true;
}

// void SPI::uninitialise()
// {
//   Logger log(1, 1);
//   log.ERR("SPI", "uninitialising");

//   int fd;
//   fd = open("/sys/class/gpio/unexport", O_WRONLY);    // not sure here
//   if (fd < 0) {
//     log.ERR("SPI", "could not open unexport");
//     return;
//   }

//   char buf[10];
//   for (uint32_t pin : exported_pins) {
//     snprintf(buf, sizeof(buf), "%d", pin);
//     write(fd, buf, strlen(buf) + 1);
//   }
//   close(fd);
// }


void SPI::setClock(Clock clk)
{
  uint32_t data;
  switch (clk) {
    case Clock::k1MHz:  data = 1000000;   break;
    case Clock::k4MHz:  data = 4000000;   break;
    case Clock::k20MHz: data = 20000000;  break;
  }

  if (ioctl(spi_fd_, SPI_IOC_WR_MAX_SPEED_HZ, &data) < 0) {
    log_.ERR("SPI", "could not set clock frequency of %d", data);
  }
}

void SPI::transfer(uint8_t* tx, uint8_t* rx, uint16_t len)
{
#define SPI_FS  0

  if (spi_fd_ < 0) return;  // early exit if no spi device present
#if SPI_FS
  spi_ioc_transfer message = {};

  message.tx_buf = reinterpret_cast<uint64_t>(tx);
  message.rx_buf = reinterpret_cast<uint64_t>(rx);
  message.len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(1), &message) < 0) {
    log_.ERR("SPI", "could not submit TRANSFER message");
  }
#else
  if (!transfer_init_) {
    spi_ioc_transfer message = {};

    message.tx_buf = reinterpret_cast<uint64_t>(tx);
    message.rx_buf = reinterpret_cast<uint64_t>(rx);
    message.len    = len;

    if (ioctl(spi_fd_, SPI_IOC_MESSAGE(1), &message) < 0) {
      log_.ERR("SPI", "could not submit TRANSFER message");
    }
    transfer_init_ = true;
  }
  
  // 0x130
  // while(*data_ & 0x4){}  // dereference pointer
  // uint32_t* write_buffer =reinterpret_cast<uint32_t*>(base + 0x138);    // offset address of register
  uint32_t base_0 = reinterpret_cast<uint32_t>(base_mapping_[0]); // + 0x138
  log_.INFO("SPI_TEST", "base addess: 0x%x", base_0);
  ch0 = reinterpret_cast<channel*>(base_0 + 0x12C);  

  for(uint16_t x = 0; x<len; x++){
    // log_.INFO("SPI_TEST","channel 0 status before: %d", 10);
    //while(!(ch0->status & 0x2));
    log_.INFO("SPI_TEST","Status register: %x", ch0->status);
    ch0->control = ch0->control | 0x1;
    ch0->config = ch0->config & 0xfffcffff;
    ch0->tx_buf = tx[x]; 
    log_.INFO("SPI_TEST","Status register: %x", ch0->status);

    log_.INFO("SPI_TEST","Config register: %x", ch0->config);
    

    // ch0->control = 0x1;
    log_.INFO("SPI_TEST","Control register: %x", ch0->control);
    
    while(!(ch0->status & 0x1))
    {
      utils::concurrent::Thread::sleep(1000);
      log_.INFO("SPI_TEST","Status register: %p", &ch0->status);
    }
    log_.INFO("SPI_TEST","Status register: %d", ch0->status);
    // log_.INFO("SPI_TEST","Read buffer: %d", ch0->rx_buf);
    
    // log_.INFO("SPI_TEST","channel 0 status after: %d", 10);
    // write_buffer++;
  }  

#endif
}

void SPI::read(uint8_t addr, uint8_t* rx, uint16_t len)
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  spi_ioc_transfer message[2] = {};

  // send address
  message[0].tx_buf = reinterpret_cast<uint64_t>(&addr);
  message[0].rx_buf = 0;
  message[0].len    = 1;

  // receive data
  message[1].tx_buf = 0;
  message[1].rx_buf = reinterpret_cast<uint64_t>(rx);
  message[1].len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(2), message) < 0) {
    log_.ERR("SPI", "could not submit 2 TRANSFER messages");
  }
}

void SPI::write(uint8_t addr, uint8_t* tx, uint16_t len)
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  spi_ioc_transfer message[2] = {};
  // send address
  message[0].tx_buf = reinterpret_cast<uint64_t>(&addr);
  message[0].rx_buf = 0;
  message[0].len    = 1;

  // write data
  message[1].tx_buf = reinterpret_cast<uint64_t>(tx);
  message[1].rx_buf = 0;
  message[1].len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(2), message) < 0) {
    log_.ERR("SPI", "could not submit 2 TRANSFER messages");
  }
}

SPI::~SPI()
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  close(spi_fd_);
}
}}}   // namespace hyped::utils::io
