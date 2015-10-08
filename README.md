# Sludge

Sludge is an archiving utility inspired by tar and the like. Tape ARchive, or tar, is a file format and a utility used to store and manipulate collections of files.
The original intention behind the tar archive was to aggregate the contents of many files to improve the read/write performance of tape drives. 
These days we use it to create archives to store and transmit multiple files as a single file.

Sludge is able to recognize command-line options for creating a new archive with a specified name and a list of files,
adding to an existing archive with a specified name and a list of files, removing a file from an existing archive with a specified name and list of files, 
listing the contents of an existing archive, and extracting some or all of the files from an existing archive. 
See the usage examples for expected functionality.

-----------------------
Sludge Usage Examples
-----------------------
Create an archive called archive name.sludge that contains the contents of file.1, file.2, and file.3:

./sludge -c archive_name.sludge file.1 file.2 file.3

List the contents of the archive name.sludge:

./sludge -l archive_name.sludge

Add files file.4 and file.5 to archive name.sludge:

./sludge -a archive_name.sludge file.4 file.5

Remove file.3 from archive name.sludge (after this operation, the space that file.3 was occupying in the archive should be reclaimed, i.e. if you look at the size of archive name.sludge,
it should be smaller after this operation than it was before, given that the le exists,
unless another file remaining in the archive is also associated with that data.):

./sludge -r archive_name.sludge file.3

Extract all files from archive name.sludge:

./sludge -e archive_name.sludge

Extract only file.2 and file.5 from archive name.sludge:

./sludge -e archive_name.sludge file.2 file.5
