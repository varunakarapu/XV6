PROJECT- TEAM MEMBERS ARE
VARUN AKARAPU
HARSHINI MALLADI
HARSHINI POLSANI

Installed xv6 on Ubuntu on Windows using WSL

Instructions:-
Kindly execute using sudo command 
Also if sometimes it might ask for permissions please grant permission using chmod +X
make clean before running test files


PART 1:- lseek()
We have implemented lseek functionality which ignores the offset and considers to print from that point(only the SEEK_SET). This involves adding a new system call called lseek in syscall.c. We also add it to syscall.h and assign they next available number for the system call. The code checks the condition whether 2 arguments are passed or not, if not it returns -1. The offset is assigned to f->off of file struct.

testlseek:
we write the contents into a file and define an offset value. When we read the file back again after lseek, it gives the result from the offset index till the end of the string.


PART 2:- Adding Symbolic Links

Here we created a system call named sys_symlink()
Firstly we need to modify the stat.h by including T_SYMLINK
Later we have also added (O_NOFOLLOW) flag in fcntl.h 
Whenever symlink() function is called, sys_symlink() is called which is implemented in sysfile.c where we take arguments of path and target,
namei function is used to get the inode information from the path
If it doesnot exit we call the create function by sending the path and and T_SYMLINK so that it is created
We also made sure that symbolic link opens the symlink when O_NOFOLLOW flag is mentioned  
Later the function writes destination file's name to inode
We have changed the open function to handle the T_SYMLINK case.


testsymlink:
write the data to an oldfile and points it to the newfile. When data is read from the newfile, it showcases the data.
- Finally unlink the newFile.

PART 3
support for large files


As mentioned in the canvas, we will change the FSSIZE to 200000 and tried running ./mkfs xv6.img and output is nmeta 107 (boot, super, log blocks 30 inode blocks 26, bitmap blocks 49) blocks 199893 total 200000

Firstly we make changes in fs.h to change the number of Direct blocks, Indirect blocks by updating NDIRECT AND NINDIRECT and change the MAXFILE to support double indirect block

Also we change addrs in file.h from [NDIRECT+1] to [NDIRECT+2] to support double indirect blocks.	

Later the bmap is updated to handle double indirect block where we write the case to handle the double indirect block similar to that of existing bmap functionality with some new variables to store extended indirect block data.

Also similarly the itrunc is also modified to handle the double-indirect blocks.


testbigfile:
We have written a testfile to write 16523 blocks

The index starts from 1 (in the code it is zero, but for demonstration we start from 1).
Prints writing into block for every 1000th iteration and also at the 16523th block
Also reads from the same file after writing by chunk.


PART 4
support for Extent files

Firstly we have added T_EXTENT file type in stat.h. Also, we add a new flag O_EXTENT to handle extend-based files.Also we add addrs[13] to the structure of stat.

We modify the open function to check the flag O_extent and handle the extent accordingly by creating the file with T_EXTENT flag.

In the bmap, if check if the type is T_EXTENT and based on that we go through all the addresses and extract the length using the AND operation.

We used balloc for allocating a new block

testextent:
We have written a stat2 function which prints out all the information about the extent such as Inode Number, Number of links and size.

