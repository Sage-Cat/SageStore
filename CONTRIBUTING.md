# Contributing to SageStore

## Getting Started

Before you begin:
- Ensure you have a GitHub account.
- Familiarize yourself with the project structure and documentation.
- If you find a bug or have a feature request, search the [Issues](https://github.com/Sage-Cat/SageStore/issues) to see if it has already been reported. If not, feel free to open a new issue.

## Branching Guidelines

- All branch names should start with the ticket number from our tracking system, followed by a short descriptor using kebab-case. Example: `SS-12-about-something`.

## Commit Messages

Commit messages should clearly describe what has been done and what is left to do, providing a concise summary of the changes. Here’s a format to follow:

- Start your commit message with a brief summary of the implementation changes (should have concrete places where something is done, like "client:" if it's client functionality only).
- Use the body of the message to provide a more detailed description. Example:
  ```
  client: ApiManager: add some functionality

  We need to have something1. 
  
  Some additional details.
  ```

- In case of a bug it's needed to have more appropriate message:

  ```
  client: ApiManager: correct some logic

  We have something not working. We need to have to fix this. 

  Some additional details.
  ```
- Reference the ticket number at the end of your commit message to link the commit to the issue tracker.

## Pull Request Process

1. **Creating a Pull Request**:
    - When you're ready to submit your changes, push your branch to GitHub and [create a pull request](https://github.com/Sage-Cat/SageStore/pulls).
    - Provide a clear description in the pull request. Mention the ticket number and a brief overview of the changes.

2. **Review Process**:
    - At least two team members must review and approve the pull request before it can be merged.
    - Reviewers should focus on the [code convention](TODO) and whether the PR adequately addresses the issue.
    - CI pipeline must succeed.

3. **Merging**:
    - After two approvals, you may merge the pull request (do not squash or trunc commits in any way).

## Code of Conduct

- Be respectful to other project contributors.
- Ensure cross-platform compatibility for every change that's accepted. Currently supported OS could be found at the README.md.
- Keep all your interactions professional, and respect privacy and security guidelines.
