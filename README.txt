Add revision support to users and fields like there is for nodes.

For the moment all field from {users} have revisions, except password.

What's working:
* Create/edit users
* Fields on users
* Show revisions
* Revert revision
* Delete revision
* Permissions: view, revert and delete
* Views integration

BTW: I'll appreciate all help I can get, so feel free to:
* Test
* Code review
* Patch
* ....

Version 2
---------

Co-operates with version 2 of Profile2, which incorporates revisions and diffs,
creating a consistent UI when both are in use. Some menu paths have been changed
to achieve this.

Adds a User Revision Diff sub-module to provide revision diffs. This is based on
the code of the separate User Diff module, and replaces it.
If you are already using User Diff you should disable it and enable User Revision Diff
instead. User Revision Diff integrates more full with the Diff module, in particular
making use of the "Users" settings under the Diff settings Entities tab
(admin/config/content/diff/entities), and uses Diff's formatting and themeing
for a consistent UI.
