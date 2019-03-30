# Guide for Git

We use GitHub for version control of the Hyped project so all subteams of software can add their documents and source files. The main repository is made up of the *develop* branch and the *master* branch. All working code is merged to the *develop* branch.

  - If you have not yet completed the Git workshop, complete Activity 1 and Activity 2 [here](https://drive.google.com/drive/u/0/folders/15_37llIRtUauiuanBvjv7l61TpLYvi1s). You will not be added as a contributor until you have completed the workshop.
  - For a thorough tutorial and cheat sheet, click [here](https://www.atlassian.com/git/tutorials/atlassian-git-cheatsheet).
  - The rest of this guide will cover import topics pertinent to the Hyped repo, starting with a quick cheat sheet.

### Quick reminders
- Adding, committing, and pushing changes: always check with `git status`
```
$ git status
$ git add <file1> <file2> ...
$ git commit -m ‘<commit_message>’
```
- Branching: always check which branch you are in when committing
```
$ git branch
$ git checkout -b <your-new-branch>
$ git branch
```
### GitFlow

  - In the *hyped-2019* repo, you cannot push directly to the *develop* branch
  1. **To push edits, you must create a new branch:**
  - Branch name format: [subteam_abbreviation]-[purpose]
    - mgt - Management (Heads)
    - stm - State Machine
    - nav - Navigation
    - tlm - Telemetry
    - lib - Libraries
    - mot - Motor Control
    - sns - Sensors tst - Testing

```
$ git checkout develop
$ git checkout -b <branch_name>
$ git push -u origin <branch_name>
```
2. **Build your feature:**
-- Switch to your branch:
```
$ git checkout <branch_name>
```
-- If other people are working on the feature pull their changes and thus avoid future conflicts:
```
$ git pull --rebase
```
-- If you have conflicts [resolve them](https://help.github.com/articles/resolving-a-merge-conflict-using-the-command-line/) and continue rebase (Remember not to commit):
```
$ git add <file1> <file2> ...
$ git rebase --continue
```
-- Add and commit your changes to your branch:
```
$ git status
$ git add
$ git status
$ git commit -m "<commit_message>"
```
-- Push your branch:
```
$ git push origin <branch_name>
```
3. **Open a new pull request in the [repo](https://github.com/Hyp-ed/hyped-2019) from your feature branch to the *develop* branch**
-- For the repo, every pull request requires at least one reviewer, ideally someone from your subteam.
-- Reviewers should take the time to understand the code even if they trust the contributor. If there are flaws in the functionality and style DO NOT APPROVE. Request the necessary changes by commenting on the feature files of the branch and approve once they have been pushed.
-- Make sure to assign a reviewer to ensure the approval of your pull request.
-- For more information, read the [README.md](https://github.com/Hyp-ed/hyped-2019/blob/26e6e39a1a1a3af0e63ca6fd73b75fa4c3336693/README.md) file.

4. **Merge your pull request**
-- Once your pull request is approved, you must merge your feature branch into the *develop* branch.
-- Make sure to delete your local and remote branch once merged:
```
$ git branch -d <branch_name>
$ git push <remote_name> --delete <branch_name>
```

