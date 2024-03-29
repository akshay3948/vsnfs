/*
 * CSE506 - VSNFS
 *
 * vsnfsClient.h --
 *
 *      VSNFS client headers
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */
#ifndef _VSNFSCLIENT_H
#define _VSNFSCLIENT_H
#include <linux/module.h>
#include <linux/socket.h>

/* Some callers of 'ls' use the file block size returned by a stat of a
 * directory as the size of buffer supplied to 'ls'. Sizes smaller that 4096
 * might result in more READDIR calls to the server than we need
 */

#define VSNFS_DEFAULT_DISK_BLOCK_SIZE	512
#define VSNFS_DEFAULT_FILE_BLOCK_SIZE	4096

/* The maximum size of the RPC reply header and VSNFS reply header for
 * a READ or WRITE request. Since we know these sizes are fixed this keeps
 * us from reading more data out of the socket than we need to
 */

#define VSNFS_MAX_IO_HEADER_SIZE	256

/* Client Initialization routines */
extern int VSNFSClientInit(void);
extern int VSNFSClientCleanup(void);

/*
 * superblock operations in vsnfsClient.c
 */

extern struct inode *vsnfs_alloc_inode(struct super_block *sb);
extern void vsnfs_destroy_inode(struct inode *);
extern struct inode *vsnfs_fhget(struct super_block *, struct vsnfs_fh *);

/*
 * vsnfs/file.c
 */

extern const struct inode_operations vsnfs_file_inode_operations;
extern const struct file_operations vsnfs_file_operations;

/*
 * vsnfs/dir.c
 */

extern const struct inode_operations vsnfs_dir_inode_operations;
extern const struct file_operations vsnfs_dir_operations;
extern const struct dentry_operations vsnfs_dentry_operations;
extern int vsnfs_permission(struct inode *, int);

extern int vsnfs_do_lookup(struct inode *, struct qstr *, struct vsnfs_fh *);
#endif
