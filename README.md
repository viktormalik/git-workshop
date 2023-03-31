# Advanced Git Demonstration Workshop

A workshop demonstrating some advanced concepts of Git. Instead on focusing on
basic Git functionality, such as commits and branches, it concentrates on
slightly more advanced things such as:

- best practices in developer synchronisation (pull requests)
- interactive rebase (`git rebase -i`)
- resolving merge conflicts (`git mergetool`)
- moving references (`git reset`)
- cherry-picking commits (`git cherry-pick`)
- bisecting (`git bisect`)

We have prepared a model project and several scenarios which demonstrate usage
of these concepts. This README describes the recommended workshop flow.

The model project is written in C and it implements a functionality similar to
the Unix `wc` utility - at the beginning it contains just character counting and
additional functions are added during the workshop.

The workshop was prepared by:
- Viktor Malík <vmalik@redhat.com>
- Petr Stodůlka <pstodulk@redhat.com>
- Pavel Odvody <podvody@redhat.com>

It is annually held at the Faculty of Information Technology of Brno University
of Technology, as a part of the [Practical Aspects of Software Design
](https://www.fit.vut.cz/study/course/231042/) course.

The materials are licensed under the MIT license and you are free to reproduce
the workshop using the instructions below.

The workshop is accompanied with slides that illustrate the explained concepts.
Slides are in LaTeX and require `xelatex` for building. For convenience, we also
keep the PDF with the latest slides version in the repository.

## Prerequisites

We assume that the workshop audience is familiar with basic concepts of Git,
especially with:
- creating commits (`git add`, `git commit`)
- branching (`git branch`, `git checkout`, `git reset`)
- inspecting the state of the repository (`git log`, `git status`, `git show`).

The workshop leaders should have experience with all commands demonstrated
throughout the workshop. It is especially useful to be comfortable with
resetting and cherry-picking as that will greatly improve the experience and
help you when something goes wrong (which definitely will, trust me).

Also, the audience should have a knowledge of C, at least at the level of being
able to understand written code.

We suggest to have prepared (e.g. printed) a git cheat sheet for better
orientation / overview of git commands. Below are two widely used sheets,
but you can find many more sheets or create your own one:
- https://www.atlassian.com/git/tutorials/atlassian-git-cheatsheet
- https://pbs.twimg.com/media/E-zGIcfXoAQx4Im?format=jpg&name=medium

## Environment

Since one of the goals is to demonstrate team members synchronisation, we
recommend running this workshop in 2 persons. Each of them should have their own
terminal session, ideally both sessions should be visible to the audience during
the workshop.

It is of course possible to run the workshop in one person, although we still
recommend to use 2 terminal sessions to clearly separate the two "virtual" team
members.

We will refer to the two team members as **A** and **B**.

## Setup

First of all, one of the team members (in our case **A**) should create a new
repo by forking or duplicating (please do not just clone) this repository. Then,
setup the initial state of the workshop by resetting `master` to the
`gws_master_initial` branch (`git reset --hard master gws_master_initial`).

The initial state of the project contains 4 commits to start from:
- basic program for character counting + `Makefile`
- a testing file
- validation of the number of CLI arguments
- `.gitignore`

**B** should clone the new repo (do not create a fork, yet).

## Scenarios

These are designed to be run in the given order, however, most of the scenarios
have a branch with the expected initial state, so it should be possible to run
them individually if necessary.

Note that scenarios contain instructions for workshop leaders which are
denoted with *\*italic text*.

### Basic team synchronisation (how not to do it)

**Goal:** First, we show how to do a basic team synchronisation using a
single branch and a single repo. The goal is to highlight problems related to
this approach and demonstrate that this is not a good way to work with Git.

**Description:** Each of the team members implements a new feature in his local
`master` branch and pushes it directly to the repo. The second one to push (**A** in
our example) will get a merge conflict that he has to resolve.

**Initial branch:** `gws_master_initial`

**Workflow:**
- **A** implements a new feature - line counting.
  - *\*Reset to `gws_master_line_count` or cherry-pick `0f94b1c`*.
- In the meantime, **B** also implements a new feature - word counting.
  - *\*Reset to `gws_master_word_count` or cherry-pick `9998ae8`*.
- **B** pushes his new commit to the repo. Everything goes fine.
- **A** tries to push his new commit to the repo - push is rejected.
- **A** runs `git pull` - get a merge conflict.
  - *\*You should have `pull.rebase=false` in your Git config or use `git pull --no-merge` instead.*
- **A** resolves the conflict using `git mergetool` (we recommend using
  [meld](https://meldmerge.org/)). This introduces a merge commit.
- **A** pushes his master to the GitHub repo.

**Conclusion:** This demonstrates bad practices going against modern Git
workflow - changes should be developed in separate branches, unnecessary merge
commits should not occur, there should be no direct pushes to `master`.

### Modern team synchronisation (how to do it)

**Goal:** We now show how to contribute to repositories in a good way. We also
demonstrate how to override (amend) the last commit.

**Description:** **B** is going to implement a new feature (help) by using a pull
request.

**Initial branch:**

**Workflow:**
- **B** forks the repository on GitHub.
- **B** adds his forked repo as a new remote (`git remote add <name> <url>`).
  - Best practice is to name your fork `origin` (as that is the one you will
    be pushing to).
- **B** creates a new branch (e.g. `help`).
- **B** implements a help option. *Cherry-pick 6771f5d but introduce a typo there.
  The best approach is to prepare a format-patch with the typo which you just
  apply*).
  - *\*To create the initial format patch for the mentioned commit: `git format-patch -1 6771f5d`*
- **B** pushes the branch to his fork,
- **B** opens a pull request (PR).
- **A** reviews the PR and discovers a typo.
- **B** fixes the typo by changing the last commit (`git commit --amend`) and does a
  force-push to the branch (`git push --force`).
  - Warning: Use the push-force just in side-branches - ideally in your fork only. 
  - You can also use `--force-with-lease` to inhibit the push into a remote
    branch containing non-fetched changes.
- **A** approves the PR and merges it to `master`:
  - There are 3 options for merging PRs: merge, rebase, and squash. We prefer
    (and will use) rebase as it preserves the original commits but does not
    create an additional merge commit.
  - Warning: rebase changes hashes of the commits.

**Conclusion:** This demonstrates the typical modern workflow - fork, implement
a feature in a new branch, open a PR, amend commits if necessary, and merge the
PR.

**Final branch:** `gws_add_help`

### Rebase and interactive rebase

**Goal:** In this scenario, we show basic use-cases for `git reset`, `git
rebase`, and `git rebase --interactive`.

**Description:** **A** is going to implement a new feature (counting by custom
separator). In the middle of the feature development, **A** figures out a new
optimization which could be merged into `master` before the current feature is
finished. Therefore, **A** first implements the optimization and then rebases his
ongoing work onto that. We also demonstrate a mistake done during this rebase
and show how to fix it cleanly with interactive rebase.

**Initial branch:** `gws_add_help`

**Workflow:**
- **A** creates a new branch `own-separator`.
- **A** implements a new feature for counting by custom separator
  - *\*cherry-pick `0bc21a8`*
- **A** realizes that it would be nice to optimize CLI option checks by removing
  repetitive `strcmp` calls and factoring their results into variables. This
  feature should be merged to `master` before finishing work on the custom
  separator feature so that **B** can use it in the meantime.
- CLI option optimization:
  - **A** creates a new branch `option-opt`.
  - **A** now realizes that the current commit (one with the custom separator
    feature) should not be a part of `option-opt`. Instead of deleting and
    re-creating the branch, **A** resets it one commit back using  
    `git reset --hard HEAD^`.
  - **A** now implements CLI option optimization
    - *\*cherry-pick `e42bfa2`*.
  - **A** opens a new pull request from `option-opt`, **B** approves it, and they
    merge it to `master`. Optionally, to save time, **A** merges the branch to
    `master` directly.
- **A** now continues work on custom separator (`git checkout own-separator`).
    - **A** implements a more robust CLI options check
      - *\*cherry-pick `0080981`*
    - In order to be able to open a PR, **A** must resolve conflicts between
      `own-separator` and `master`.
    - **A** chooses to use rebase (`git rebase master`).
    - **A** resolves merge conflicts by introducing a new variable for his new CLI
      option (`s_opt`), however, **A** makes a mistake by forgetting to define
      `s_opt` in the code.
- **A** now runs `make` just to realize that the definition of `s_opt` is indeed
  missing.
    - **A** fixes the problem and commits the fix.
    - In order not to pollute the commit history with fix commits, **A** performs an
      interactive rebase (`git rebase --interactive`) to merge the fix commit
      into the first commit of `own-separator`. This is done by moving the fix
      commit below the first commit and setting it to `fixup` in the interactive
      rebase editor.
- **A** merges `own-separator` into `master` (either via PR or directly).

**Conclusion:** Interactive rebase is one of the most used features in modern
Git development. It allows to clean-up PRs before merging them.
Attention: interactive rebase changes commit SHAs and so it must never be used
on `master`.

**Final branch:** `gws_master_manual_finish`

### Rewriting history with interactive rebase

**Goal:** We demonstrate several options for rewriting commits while
incorporating changes required by PR reviews.

**Description:** **B** implements an improved CLI options parsing and opens a PR. A
performs several rounds of reviews and **B** updates his commits with the required
changes.

**Initial branch:** `gws_master_manual_finish`

**Workflow:**
- First of all, the tool now contains nice basic functionality, so the team
  decides to "release" the first version numbered 0.1. This is done by creating
  a Git tag using `git tag v0.1`.
    - On GitHub, this tag shows among *Releases*.
- **B** implements improved CLI option parsing
  - *\*cherry-pick c853f71^..43b6520*
- **B** opens a PR (may use description from
  [#9](https://github.com/viktormalik/git-workshop/pull/9))
- **A** reviews the PR and discovers a missing `NULL` in configuration
  initialization
- **B** fixes the issue by doing an interactive rebase (`git rebase -i`) and
  editing the problematic commit (*cherry-pick e7e9bd9 without committing (`-n`)
  while in interactive rebase*)
- **B** force-pushes the branch (`git push --force`)
- **A** does another review and discovers useless `continue` commands
- **B** again does the fix by interactive rebase + edit
  - *\*cherry-pick -n 8f07fae3*
  This causes a merge conflict which **B** must resolve.
- **A** does another review and discovers 2 more problems: separator should have a
  type `char` and there are missing spaces around `=` operator in the `for` loop
  header. Note that this is not a good review style - everything should be
  checked at once.
- **B** now fixes the issues by implementing them in separate commits which are
  committed as fixup commits (`git commit --fixup`). *Cherry-pick e681beb and
  02405b0. You may optionally cherry-pick with `-n` to show the commit command*.
- **B** now pushes the branch. Show that GitHub displays only changes from the last
  review (does not work when force-pushing).
- **A** reviews the PR, approves it, and requests a rebase.
- **B** now runs `git rebase --interactive --autosquash` which automatically applies
  fixup commits and force-pushes the branch.
- **A** merges the PR.

**Conclusion:** Pull request reviewers often require changes to be done to the
code. In such a case, it is a good practice to incorporate the changes directly
to the relevant commits. Optionally, it is possible to create new temporary
commits (so that it is clear what was added to the PR) and clean-up the commit
history at the end. **A** good help can be usage of fixup commits.

**Final branch:** `gws_cli_parse_fixed`

### Rebase with large conflicts

**Goal:** Once more we demonstrate usage of rebasing, this time we concentrate
on a situation when there are two open PRs with potentially large merge
conflicts. We show what is a good way to approach such situation.

**Description:** While **B** is implementing an improved CLI parsing (the previous
scenario), **A** implements another feature - handling multiple successive
whitespaces when counting words (current version of the program counts each
whitespace as a separate word). This is a rather small change.

**Initial branch:** `gws_master_manual_finish`

**Workflow:**
- **A** creates a new branch `successive-whitespace` and implements the new feature
  - *\*cherry-pick 0d5a663 or reset to `gws_words_multiple_ws_initial`*
- Now, if the previous scenario was not merged, the question is which feature to
  merge first (there will be a large conflict). A good approach is to merge the
  larger PR as the first and then rebase the smaller one.
- Hence, **A** now rebases `successive-whitespace` onto `master` containing improved
  CLI options
  - *\*`master` should be equal to `gws_cli_parse_fixed`*
- There is a lot of conflicts in the mergetool. Since the successive whitespace
  feature is rather simple, the best approach here is to merge everything from
  the other branch (`master`) first, and then manually "reconstruct" the current
  feature there.
- **A** resolves the conflict and merges the branch into `master`.
- *Note: this merge introduces a hidden bug :-)*

**Conclusion:** When having two open PRs with a potential conflict, it is
usually better to merge the one with more extensive changes and rebase the
smaller one onto that. The rebase should always be done by the person who
implemented the branch being rebased.

**Final branch:** `gws_words_mutiple_ws_fixed`

### Cherry-pick and skip in interactive rebase

**Goal:** We show practical usage of `git cherry-pick` and some consequences
that may happen when it is used. In particular, we show the necessity of using
`git rebase --skip`.

**Description:** **A** and **B** are both going to implement new features. **A** implements
support for analysing multiple files at once, **B** implements support for
recursively analysing all files in a directory. Both features will need a
similar code.

**Initial branch:** `gws_master_pre_recursion`

**Workflow:**
- **B** implements the first version of recursive directory parsing using 4 commits
  - *\*cherry-pick e13e79f^..fc2cc92*
  - Notice the header of commit messages (using X/Y). This is not used in most
    projects anymore, but it used to be relevant when patches were sent via
    email.
- In the meantime, **A** implements support for multiple files:
  - **A** realizes that 3 of the commits from **B**'s branch can be used for both
    features. Hence, **A** cherry-picks those commits (`git cherry-pick
    e13e79f^..96e2313`).
  - **A** builds his implementation on top of these
    - *\*cherry-pick 0f4a676 or reset to `gws_multiple_files_initial`*
  - **A** discovers a bug in **B**'s code - an `else if` structure should be changed
    to `if`. **A** fixes this bug by overriding the relevant commit (using
    interactive rebase).
  - During the interactive rebase, **A** also rewords the commit messages and
    removes the mentioned headers.
- **A**'s feature (*\*`gws_multiple_files_fixed`*) is merged to `master`.
- **B** now rebases his branch onto `master`.
  - Three of the commits are already in `master`. Even though Git reports a
    conflict to resolve, the correct approach is to skip them using `git
    rebase --skip`.
  - During the rebase, **B** also rewords the remaining messages.
- **B**'s feature is merged into `master`.

**Conclusion:** In some situations (especially when using cherry-pick), it may
happen that you are trying to rebase a branch which contains commits already
present in the target branch. If Git is not able to detect this by himself,
those commits should be skipped using `git rebase --skip`.

**Final branch:** `gws_recursion_fixed`

### Finding bugs with bisect

**Goal:** This scenario shows how Git can be used to locate bugs using `git
bisect`.

**Description:** One of the previous scenarios introduced a bug which causes the
program not to work on some cases. We now try to to find and fix the problem.

**Workflow:**
- **A** adds a new file for testing which reveals that the word counting with
  multiple whitespaces between words works for odd number of spaces only.
- **A** wants to find when the bug was introduced and uses `git bisect` to do that.
    - **A** starts bisecting by `git bisect start`
    - First, it is necessary to identify any past revision (commit or tag) when
      the tool worked. In our case, it is the version 0.1, so **A** marks it with
      `git bisect good v0.1`.
    - Next, **A** marks that the current revision is broken using `git bisect bad
      master`.
- **A** runs bisect by `git bisect run`. Bisect stops at some commits and **A** must
  check if the current version works and mark it using:
    - `git bisect good` if the current revision works,
    - `git bisect bad` if the current revision is broken.
- After several steps, bisect will identify a particular commit which caused the
  bug.
- **A** may now exit bisect (`git bisect reset`), fix the bug, and commit it.
- All this could be automated by providing a script which exits with 0 on a good
  revision and with 1 on a bad revision (e.g. `make test`). Then, it would be
  sufficient to run bisecting with `git bisect run <script>`.

**Conclusion:** When a bug is discovered, bisect makes it easier to locate a
commit which introduced it. In order for bisect to be effective, it is necessary
that the project is working (i.e. can be compiled and all tests pass) after each
commit.
