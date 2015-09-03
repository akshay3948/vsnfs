Network filesystem enables file access over a network through the usage of Remote Procedure Calls. Notable network filesystems include Sun's Network File System(NFS), Andrew FileSystem, Common Internet FileSystem(CIFS). The modern day network filesystems are very complex and hard to comprehend.

The motivation in taking up this project is to understand the intricacies of a filesystem by designing and  implementing a bare minimal network filesystem that caters to the user request for the remote access of files. It is always useful to have a design template which can be extended and tuned for specific requirements.

Our design is loosely based on the design of **NFSv2 specification(RFC#1094)**. VSNFS is currently being implemented as a loadable kernel module in linux kernel v2.6.31.6.