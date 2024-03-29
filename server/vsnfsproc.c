/*
 * vsnfsproc.c - contains all server side NFS procedures 
 */  
    
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include <linux/list.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"

struct vsnfs_lookup_table vsnfs_lp_tab;

/* returns 0 on success -1 on failure */ 
int vsnfsd_fh_to_path(struct vsnfs_fh *fh, char *path, int len) 
{
	/* TBD - remove the hardcoding once vsnfs_lookup_table is proper */
	
#if 0
    strcpy(path, "/temp");
	return 0;	
#endif	/*  */
	    
#if 1
	struct vsnfs_lookup_table *node;
	unsigned long ino = 0;
	ino = simple_strtoul(fh->data, 0, 0);
	if (!ino)
		return -1;
	list_for_each_entry(node, &vsnfs_lp_tab.list, list) {
		if (node->ino == ino) {
			strncpy(path, node->path, len);
			return 0;
		}
	}
	
	    /* not present in the table */ 
	    return -1;
	
#endif	/*  */
}


/* returns nfs error 
* argp - argument sent by client (after decoded by the corr xdr decode function)
* resp - result to be sent (it will be decoded by the corr xdr encode function)
*/ 
    
/* this procedure just return x+1 where x is the value received from client */ 
static __be32 
vsnfsd_proc_null(struct svc_rqst *rqstp, struct vsnfsd_nullargs *argp,
		 struct vsnfsd_nullres *resp) 
{
	vsnfs_trace(KERN_DEFAULT, ":-)\n");
	resp->dummy = argp->dummy + 1;
	return vsnfs_ok;
}

static __be32 
vsnfsd_proc_lookup(struct svc_rqst *rqstp, struct vsnfsd_lookupargs *argp,
		   struct vsnfs_fh *resp) 
{
	struct vsnfs_lookup_table *node;
	unsigned long ino = 0;
	char *path = NULL;
	char *tmp = NULL, *nullt = NULL;
	__be32 rc = vsnfs_ok;
        int len =0;
	struct nameidata *nd = NULL;
	struct dentry *dentry = NULL;
	tmp = argp->fh.data;
	ino = simple_strtoul(argp->fh.data, &tmp, 0);
	list_for_each_entry(node, &vsnfs_lp_tab.list, list) {
		if (node->ino == ino) {
			path = kmalloc(VSNFS_MAXPATHLEN + 1, GFP_KERNEL);
			if (!path) {
				rc = -vsnfserr_nospc;
				goto out_lookup;
			}
			len = strlen(node->path); 
                        memcpy(path, node->path, len + 1);
			goto skip_loop;
		}
		
		    //  vsnfs_trace(KERN_DEFAULT, "list: %s : %s\n",node->data,node->path);  
	}
      skip_loop:if (path != NULL) {
	        nullt = strchr(path, '\0');
		memcpy(nullt, argp->filename, argp->len);
		len = argp->len + len;
		path[len] = '\0';
		nd = kmalloc(sizeof(*nd), GFP_KERNEL);
		if (!nd) {
			rc = -vsnfserr_nospc;
			goto out_lookup;
		}
		memset(nd, 0, sizeof(*nd));
		rc = path_lookup(path, 0, nd);
		if (rc < 0)
			vsnfs_trace(KERN_DEFAULT, "failed in path_lookup\n");
		dentry = dget(nd->path.dentry);
		path_put(&nd->path);
		vsnfs_trace(KERN_DEFAULT, "inode no = %ld\n",
			     dentry->d_inode->i_ino);
		snprintf(resp->data, VSNFS_FHSIZE, "%ld",
			  dentry->d_inode->i_ino);
		if (S_ISREG(dentry->d_inode->i_mode))
 {
			resp->type = VSNFS_REG;
			  
}
		else if (S_ISDIR(dentry->d_inode->i_mode)) {
			resp->type = VSNFS_DIR;
			
len = strlen(path);
			
if (path[len - 1] != '/') {
				
path[len] = '/';
				
path[len + 1] = '\0';
			
}
		
}
       	        vsnfs_trace(KERN_DEFAULT, "PATH variable = %s : %d", path,
				strlen(path));

		list_for_each_entry(node, &vsnfs_lp_tab.list, list) {
		if (node->ino == dentry->d_inode->i_ino) {
			goto out_lookup_dput;
		  }
		}

		    /* Adding the ino & path to the lookup table */ 
		    node =
		    (struct vsnfs_lookup_table *)
		    kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
		node->ino = dentry->d_inode->i_ino;
		node->path = kmalloc(strlen(path) + 2, GFP_KERNEL);
		if (!node->path) {
			rc = -vsnfserr_nospc;
			goto out_lookup_dput;
		}
		memcpy(node->path,path,strlen(path) + 1);
		list_add(&(node->list), &(vsnfs_lp_tab.list));
	}
      out_lookup_dput:dput(dentry);
	kfree(nd);
      out_lookup:kfree(path);
	return rc;
}

static __be32 
vsnfsd_proc_getroot(struct svc_rqst *rqstp, struct vsnfsd_getrootargs *argp,
		    struct vsnfs_fh *resp) 
{
	struct nameidata *nd;
	struct dentry *root;
	__be32 ret = vsnfs_ok;
	struct vsnfs_lookup_table *node;
	char *path;
	vsnfs_trace(KERN_DEFAULT, "in getroot\n");
	nd = kmalloc(sizeof(*nd), GFP_KERNEL);
	if (unlikely(!nd)) {
		return -vsnfserr_nospc;
	}
	memset(nd, 0, sizeof(*nd));
	path = kmalloc((argp->len) + 2, GFP_KERNEL);
	if (unlikely(!path)) {
	        kfree(nd);
		return -vsnfserr_nospc;
	}
	memcpy(path, argp->path, argp->len);
	if (path[argp->len - 1] != '/') {
		path[argp->len] = '/';
		path[argp->len + 1] = '\0';
	}
	
	else {
		path[argp->len] = '\0';
	}

	ret = path_lookup(path, 0, nd);
	if (ret < 0)
		vsnfs_trace(KERN_DEFAULT, "failed in path_lookup: %s\n", path);
	root = dget(nd->path.dentry);
	path_put(&nd->path);
	vsnfs_trace(KERN_DEFAULT, "inode no = %ld\n", root->d_inode->i_ino);
	snprintf(resp->data, VSNFS_FHSIZE, "%ld", root->d_inode->i_ino);
	if (S_ISREG(root->d_inode->i_mode)) {
		ret = -vsnfserr_notdir;
		goto out_getroot;
	}
	
	else if (S_ISDIR(root->d_inode->i_mode))
		resp->type = VSNFS_DIR;
	vsnfs_trace(KERN_DEFAULT, "type in server = %d\n", resp->type);
	
	    /* Adding the ino & path to the lookup table */ 
	    INIT_LIST_HEAD(&vsnfs_lp_tab.list);
	node =
	    (struct vsnfs_lookup_table *)
	    kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
	node->ino = root->d_inode->i_ino;
	node->path = kmalloc((argp->len) + 2, GFP_KERNEL);
	if (!node->path) {
		ret = -vsnfserr_nospc;
		goto out_getroot;
	}
	memcpy(node->path, path, strlen(path) + 1);
	list_add(&(node->list), &(vsnfs_lp_tab.list));
      out_getroot:dput(root);
	kfree(nd);
	kfree(path);
	return ret;
}

static __be32 
vsnfsd_proc_readdir(struct svc_rqst *rqstp, struct vsnfsd_readdirargs *argp,
		    struct vsnfsd_readdirres *resp) 
{
	int count;
	__be32 vfserr;
	vsnfs_trace(KERN_DEFAULT, "\n");
	
    /* Shrink to the client read size */ 
    count = (argp->count >> 2);	/* is - 2 necessary ? */

    /* Make sure we've room for the NULL ptr & eof flag */ 
    count -= 2;
	if (count < 0)
		count = 0;

	resp->buffer = argp->buffer;	/* resp should be sent in the req buffer itself */
	resp->offset = NULL;
	resp->buflen = count;
	resp->err = vsnfs_ok;
	
    /* Read directory and encode entries on the fly */ 
    vfserr = vsnfsd_readdir(&argp->fh, resp);

	if(vfserr && resp->err == vsnfs_ok) {
		/* some vfs error */
		resp->err = vfserr;
		vsnfs_trace(KERN_DEFAULT, "vfs error\n");
		}
	else if(vfserr)
		{
		/* some filldir erro resp->err would have been filled*/		
		vsnfs_trace(KERN_DEFAULT, "filldir error\n");
		}
	else
		{		
		vsnfs_trace(KERN_DEFAULT, "everything fine\n");
		}

    /* if everything had gone as expected at this point
       resp->buffer should contain encoded dir entries */ 
    resp->count = resp->buffer - argp->buffer;

	return vfserr;
}


static __be32
vsnfsd_proc_read(struct svc_rqst *rqstp, struct vsnfsd_readargs *argp,
				       struct vsnfsd_readres  *resp)
{
	__be32	vsnfserr = vsnfs_ok;
	loff_t pos;
	
	vsnfs_trace(KERN_DEFAULT, "\n");

	/* 1 word for status and 1 word for the byte count.
	 */
	vsnfs_trace(KERN_DEFAULT, "\n");
	svc_reserve_auth(rqstp, 2 + argp->count);

	vsnfserr = vsnfsd_read(&(argp->fh), (char*)(argp->buffer), (size_t)(argp->count), &pos);
	resp->count = (unsigned long)pos;
	vsnfs_trace(KERN_DEFAULT, "%d\n", (int)resp->count);

	return vsnfserr;	
}



/*
 * VSNFS Server procedures.
 * No caching of results for any function
 */ 
    
#define ST 1		/* status */
#define FH 8		/* filehandle */
#define	AT 18		/* attributes */
    
#define RC_NOCACHE 0
static struct svc_procedure vsnfsd_procedures1[VSNFS_NRPROCS] = { 
	    [VSNFSPROC_NULL] = {
			.pc_func = (svc_procfunc) vsnfsd_proc_null, 
			.pc_decode = (kxdrproc_t) vsnfssvc_decode_nullargs,
			.pc_encode = (kxdrproc_t) vsnfssvc_encode_nullres,
			.pc_argsize = sizeof(struct vsnfsd_nullargs), 
			.pc_ressize = sizeof(struct vsnfsd_nullres), 
			.pc_cachetype = RC_NOCACHE, 
			.pc_xdrressize = ST + 1, }, 
		[VSNFSPROC_GETROOT] = {
			.pc_func = (svc_procfunc)vsnfsd_proc_getroot,
			.pc_decode = (kxdrproc_t)vsnfssvc_decode_getrootargs,
		    .pc_encode = (kxdrproc_t)vsnfssvc_encode_fhandle,
		    .pc_argsize = sizeof(struct vsnfsd_getrootargs),
		    .pc_ressize = sizeof(struct vsnfs_fh),
		    .pc_cachetype = RC_NOCACHE,
		    .pc_xdrressize = ST + FH, },
	    [VSNFSPROC_LOOKUP] = {
	    	.pc_func = (svc_procfunc) vsnfsd_proc_lookup, 
			.pc_decode = (kxdrproc_t) vsnfssvc_decode_lookupargs,
		   	.pc_encode = (kxdrproc_t) vsnfssvc_encode_fhandle, 
		   	.pc_argsize = sizeof(struct vsnfsd_lookupargs), 
		   	.pc_ressize = sizeof(struct vsnfs_fh), 
		   	.pc_cachetype = RC_NOCACHE, 
		   	.pc_xdrressize = ST + FH, }, 
		[VSNFSPROC_READDIR] = {\
			.pc_func = (svc_procfunc)vsnfsd_proc_readdir,
			.pc_decode = (kxdrproc_t)vsnfssvc_decode_readdirargs,
			.pc_encode = (kxdrproc_t)vsnfssvc_encode_readdirres,
			.pc_argsize = sizeof(struct vsnfsd_readdirargs),
			.pc_ressize = sizeof(struct vsnfsd_readdirres),
			.pc_cachetype = RC_NOCACHE,
			.pc_xdrressize = ST,
			/* check this */								
			}, 
		[VSNFSPROC_READ] = {
			.pc_func = (svc_procfunc) vsnfsd_proc_read,
			.pc_decode = (kxdrproc_t) vsnfssvc_decode_readargs,
			.pc_encode = (kxdrproc_t) vsnfssvc_encode_readres,
			//.pc_release = (kxdrproc_t) nfssvc_release_fhandle, /* <TO DO> */
			.pc_argsize = sizeof(struct vsnfsd_readargs),
			.pc_ressize = sizeof(struct vsnfsd_readres),
			.pc_cachetype = RC_NOCACHE,
			.pc_xdrressize = ST+VSNFSSVC_MAXBLKSIZE/4, /* <TO DO> check this */
		},					
			
	    /*add procs here */ 
};

struct svc_version vsnfsd_version1 = { 
	.vs_vers = 1,
	.vs_nproc = VSNFS_NRPROCS, 
	.vs_proc = vsnfsd_procedures1, 
	.vs_dispatch = vsnfsd_dispatch, 
	.vs_xdrsize = VSNFS_SVC_XDRSIZE, 
};


/* error codes */ 
/*
 * Map errnos to NFS errnos.
 */ 
__be32  vsnfserrno(int errno) 
{
	static struct {
		__be32 vsnfserr;
		int syserr;
	} vsnfs_errtbl[] = {
		 {
		vsnfs_ok, 0},  {
		vsnfserr_perm, -EPERM},  {
		vsnfserr_noent, -ENOENT},  {
		vsnfserr_io, -EIO},  {
		vsnfserr_nxio, -ENXIO},  {
		vsnfserr_acces, -EACCES},  {
		vsnfserr_exist, -EEXIST},  {
		vsnfserr_xdev, -EXDEV},  {
		vsnfserr_mlink, -EMLINK},  {
		vsnfserr_nodev, -ENODEV},  {
		vsnfserr_notdir, -ENOTDIR},  {
		vsnfserr_isdir, -EISDIR},  {
		vsnfserr_inval, -EINVAL},  {
		vsnfserr_fbig, -EFBIG},  {
		vsnfserr_nospc, -ENOSPC},  {
		vsnfserr_rofs, -EROFS},  {
		vsnfserr_mlink, -EMLINK},  {
		vsnfserr_nametoolong, -ENAMETOOLONG},  {
		vsnfserr_notempty, -ENOTEMPTY},  {
		vsnfserr_stale, -ESTALE},  {
		vsnfserr_jukebox, -ETIMEDOUT},  {
		vsnfserr_jukebox, -ERESTARTSYS},  {
		vsnfserr_io, -ETXTBSY},  {
		vsnfserr_notsupp, -EOPNOTSUPP},  {
	vsnfserr_toosmall, -ETOOSMALL}, };
	int i;
	for (i = 0; i < ARRAY_SIZE(vsnfs_errtbl); i++) {
		if (vsnfs_errtbl[i].syserr == errno)
			return vsnfs_errtbl[i].vsnfserr;
	}
	printk(KERN_INFO "vsnfsd: non-standard errno: %d\n", errno);
	return vsnfserr_io;
}


