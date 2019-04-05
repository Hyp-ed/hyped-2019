// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#ifndef PROTOBUF_INCLUDED_message_2eproto
#define PROTOBUF_INCLUDED_message_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_message_2eproto 

namespace protobuf_message_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_message_2eproto
namespace telemetry_data {
class ServerToClient;
class ServerToClientDefaultTypeInternal;
extern ServerToClientDefaultTypeInternal _ServerToClient_default_instance_;
class TestMessage;
class TestMessageDefaultTypeInternal;
extern TestMessageDefaultTypeInternal _TestMessage_default_instance_;
}  // namespace telemetry_data
namespace google {
namespace protobuf {
template<> ::telemetry_data::ServerToClient* Arena::CreateMaybeMessage<::telemetry_data::ServerToClient>(Arena*);
template<> ::telemetry_data::TestMessage* Arena::CreateMaybeMessage<::telemetry_data::TestMessage>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace telemetry_data {

enum TestMessage_Command {
  TestMessage_Command_ERROR = 0,
  TestMessage_Command_VELOCITY = 1,
  TestMessage_Command_ACCELERATION = 2,
  TestMessage_Command_BRAKE_TEMP = 3,
  TestMessage_Command_FINISH = 4,
  TestMessage_Command_EM_STOP = 5,
  TestMessage_Command_TestMessage_Command_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  TestMessage_Command_TestMessage_Command_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool TestMessage_Command_IsValid(int value);
const TestMessage_Command TestMessage_Command_Command_MIN = TestMessage_Command_ERROR;
const TestMessage_Command TestMessage_Command_Command_MAX = TestMessage_Command_EM_STOP;
const int TestMessage_Command_Command_ARRAYSIZE = TestMessage_Command_Command_MAX + 1;

const ::google::protobuf::EnumDescriptor* TestMessage_Command_descriptor();
inline const ::std::string& TestMessage_Command_Name(TestMessage_Command value) {
  return ::google::protobuf::internal::NameOfEnum(
    TestMessage_Command_descriptor(), value);
}
inline bool TestMessage_Command_Parse(
    const ::std::string& name, TestMessage_Command* value) {
  return ::google::protobuf::internal::ParseNamedEnum<TestMessage_Command>(
    TestMessage_Command_descriptor(), name, value);
}
enum ServerToClient_Command {
  ServerToClient_Command_ACK = 0,
  ServerToClient_Command_STOP = 1,
  ServerToClient_Command_LAUNCH = 2,
  ServerToClient_Command_RESET = 3,
  ServerToClient_Command_RUN_LENGTH = 4,
  ServerToClient_Command_SERVICE_PROPULSION = 5,
  ServerToClient_Command_ServerToClient_Command_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ServerToClient_Command_ServerToClient_Command_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool ServerToClient_Command_IsValid(int value);
const ServerToClient_Command ServerToClient_Command_Command_MIN = ServerToClient_Command_ACK;
const ServerToClient_Command ServerToClient_Command_Command_MAX = ServerToClient_Command_SERVICE_PROPULSION;
const int ServerToClient_Command_Command_ARRAYSIZE = ServerToClient_Command_Command_MAX + 1;

const ::google::protobuf::EnumDescriptor* ServerToClient_Command_descriptor();
inline const ::std::string& ServerToClient_Command_Name(ServerToClient_Command value) {
  return ::google::protobuf::internal::NameOfEnum(
    ServerToClient_Command_descriptor(), value);
}
inline bool ServerToClient_Command_Parse(
    const ::std::string& name, ServerToClient_Command* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ServerToClient_Command>(
    ServerToClient_Command_descriptor(), name, value);
}
// ===================================================================

class TestMessage : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:telemetry_data.TestMessage) */ {
 public:
  TestMessage();
  virtual ~TestMessage();

  TestMessage(const TestMessage& from);

  inline TestMessage& operator=(const TestMessage& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  TestMessage(TestMessage&& from) noexcept
    : TestMessage() {
    *this = ::std::move(from);
  }

  inline TestMessage& operator=(TestMessage&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const TestMessage& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const TestMessage* internal_default_instance() {
    return reinterpret_cast<const TestMessage*>(
               &_TestMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(TestMessage* other);
  friend void swap(TestMessage& a, TestMessage& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline TestMessage* New() const final {
    return CreateMaybeMessage<TestMessage>(NULL);
  }

  TestMessage* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<TestMessage>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const TestMessage& from);
  void MergeFrom(const TestMessage& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(TestMessage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef TestMessage_Command Command;
  static const Command ERROR =
    TestMessage_Command_ERROR;
  static const Command VELOCITY =
    TestMessage_Command_VELOCITY;
  static const Command ACCELERATION =
    TestMessage_Command_ACCELERATION;
  static const Command BRAKE_TEMP =
    TestMessage_Command_BRAKE_TEMP;
  static const Command FINISH =
    TestMessage_Command_FINISH;
  static const Command EM_STOP =
    TestMessage_Command_EM_STOP;
  static inline bool Command_IsValid(int value) {
    return TestMessage_Command_IsValid(value);
  }
  static const Command Command_MIN =
    TestMessage_Command_Command_MIN;
  static const Command Command_MAX =
    TestMessage_Command_Command_MAX;
  static const int Command_ARRAYSIZE =
    TestMessage_Command_Command_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Command_descriptor() {
    return TestMessage_Command_descriptor();
  }
  static inline const ::std::string& Command_Name(Command value) {
    return TestMessage_Command_Name(value);
  }
  static inline bool Command_Parse(const ::std::string& name,
      Command* value) {
    return TestMessage_Command_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // .telemetry_data.TestMessage.Command command = 1;
  void clear_command();
  static const int kCommandFieldNumber = 1;
  ::telemetry_data::TestMessage_Command command() const;
  void set_command(::telemetry_data::TestMessage_Command value);

  // int32 data = 2;
  void clear_data();
  static const int kDataFieldNumber = 2;
  ::google::protobuf::int32 data() const;
  void set_data(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:telemetry_data.TestMessage)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  int command_;
  ::google::protobuf::int32 data_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_message_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ServerToClient : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:telemetry_data.ServerToClient) */ {
 public:
  ServerToClient();
  virtual ~ServerToClient();

  ServerToClient(const ServerToClient& from);

  inline ServerToClient& operator=(const ServerToClient& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ServerToClient(ServerToClient&& from) noexcept
    : ServerToClient() {
    *this = ::std::move(from);
  }

  inline ServerToClient& operator=(ServerToClient&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ServerToClient& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ServerToClient* internal_default_instance() {
    return reinterpret_cast<const ServerToClient*>(
               &_ServerToClient_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(ServerToClient* other);
  friend void swap(ServerToClient& a, ServerToClient& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ServerToClient* New() const final {
    return CreateMaybeMessage<ServerToClient>(NULL);
  }

  ServerToClient* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ServerToClient>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ServerToClient& from);
  void MergeFrom(const ServerToClient& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ServerToClient* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef ServerToClient_Command Command;
  static const Command ACK =
    ServerToClient_Command_ACK;
  static const Command STOP =
    ServerToClient_Command_STOP;
  static const Command LAUNCH =
    ServerToClient_Command_LAUNCH;
  static const Command RESET =
    ServerToClient_Command_RESET;
  static const Command RUN_LENGTH =
    ServerToClient_Command_RUN_LENGTH;
  static const Command SERVICE_PROPULSION =
    ServerToClient_Command_SERVICE_PROPULSION;
  static inline bool Command_IsValid(int value) {
    return ServerToClient_Command_IsValid(value);
  }
  static const Command Command_MIN =
    ServerToClient_Command_Command_MIN;
  static const Command Command_MAX =
    ServerToClient_Command_Command_MAX;
  static const int Command_ARRAYSIZE =
    ServerToClient_Command_Command_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Command_descriptor() {
    return ServerToClient_Command_descriptor();
  }
  static inline const ::std::string& Command_Name(Command value) {
    return ServerToClient_Command_Name(value);
  }
  static inline bool Command_Parse(const ::std::string& name,
      Command* value) {
    return ServerToClient_Command_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // .telemetry_data.ServerToClient.Command command = 1;
  void clear_command();
  static const int kCommandFieldNumber = 1;
  ::telemetry_data::ServerToClient_Command command() const;
  void set_command(::telemetry_data::ServerToClient_Command value);

  // float run_length = 2;
  void clear_run_length();
  static const int kRunLengthFieldNumber = 2;
  float run_length() const;
  void set_run_length(float value);

  // bool service_propulsion = 3;
  void clear_service_propulsion();
  static const int kServicePropulsionFieldNumber = 3;
  bool service_propulsion() const;
  void set_service_propulsion(bool value);

  // @@protoc_insertion_point(class_scope:telemetry_data.ServerToClient)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  int command_;
  float run_length_;
  bool service_propulsion_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_message_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// TestMessage

// .telemetry_data.TestMessage.Command command = 1;
inline void TestMessage::clear_command() {
  command_ = 0;
}
inline ::telemetry_data::TestMessage_Command TestMessage::command() const {
  // @@protoc_insertion_point(field_get:telemetry_data.TestMessage.command)
  return static_cast< ::telemetry_data::TestMessage_Command >(command_);
}
inline void TestMessage::set_command(::telemetry_data::TestMessage_Command value) {
  
  command_ = value;
  // @@protoc_insertion_point(field_set:telemetry_data.TestMessage.command)
}

// int32 data = 2;
inline void TestMessage::clear_data() {
  data_ = 0;
}
inline ::google::protobuf::int32 TestMessage::data() const {
  // @@protoc_insertion_point(field_get:telemetry_data.TestMessage.data)
  return data_;
}
inline void TestMessage::set_data(::google::protobuf::int32 value) {
  
  data_ = value;
  // @@protoc_insertion_point(field_set:telemetry_data.TestMessage.data)
}

// -------------------------------------------------------------------

// ServerToClient

// .telemetry_data.ServerToClient.Command command = 1;
inline void ServerToClient::clear_command() {
  command_ = 0;
}
inline ::telemetry_data::ServerToClient_Command ServerToClient::command() const {
  // @@protoc_insertion_point(field_get:telemetry_data.ServerToClient.command)
  return static_cast< ::telemetry_data::ServerToClient_Command >(command_);
}
inline void ServerToClient::set_command(::telemetry_data::ServerToClient_Command value) {
  
  command_ = value;
  // @@protoc_insertion_point(field_set:telemetry_data.ServerToClient.command)
}

// float run_length = 2;
inline void ServerToClient::clear_run_length() {
  run_length_ = 0;
}
inline float ServerToClient::run_length() const {
  // @@protoc_insertion_point(field_get:telemetry_data.ServerToClient.run_length)
  return run_length_;
}
inline void ServerToClient::set_run_length(float value) {
  
  run_length_ = value;
  // @@protoc_insertion_point(field_set:telemetry_data.ServerToClient.run_length)
}

// bool service_propulsion = 3;
inline void ServerToClient::clear_service_propulsion() {
  service_propulsion_ = false;
}
inline bool ServerToClient::service_propulsion() const {
  // @@protoc_insertion_point(field_get:telemetry_data.ServerToClient.service_propulsion)
  return service_propulsion_;
}
inline void ServerToClient::set_service_propulsion(bool value) {
  
  service_propulsion_ = value;
  // @@protoc_insertion_point(field_set:telemetry_data.ServerToClient.service_propulsion)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace telemetry_data

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::telemetry_data::TestMessage_Command> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::telemetry_data::TestMessage_Command>() {
  return ::telemetry_data::TestMessage_Command_descriptor();
}
template <> struct is_proto_enum< ::telemetry_data::ServerToClient_Command> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::telemetry_data::ServerToClient_Command>() {
  return ::telemetry_data::ServerToClient_Command_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_message_2eproto
