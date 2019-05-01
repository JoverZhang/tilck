/* SPDX-License-Identifier: BSD-2-Clause */

typedef struct {

   fs_handle_base *h;
   struct linux_dirent64 *user_dirp;
   u32 buf_size;
   u32 offset;
   off_t off;
   struct linux_dirent64 ent;

} vfs_getdents_ctx;

static inline unsigned char
vfs_type_to_linux_dirent_type(enum vfs_entry_type t)
{
   static const unsigned char table[] =
   {
      [VFS_NONE]        = DT_UNKNOWN,
      [VFS_FILE]        = DT_REG,
      [VFS_DIR]         = DT_DIR,
      [VFS_SYMLINK]     = DT_LNK,
      [VFS_CHAR_DEV]    = DT_CHR,
      [VFS_BLOCK_DEV]   = DT_BLK,
      [VFS_PIPE]        = DT_FIFO,
   };

   ASSERT(t != VFS_NONE);
   return table[t];
}

static int vfs_getdents_cb(vfs_dent64 *vde, void *arg)
{
   vfs_getdents_ctx *ctx = arg;

   const u16 entry_size = sizeof(struct linux_dirent64) + vde->name_len;
   struct linux_dirent64 *user_ent;

   if (ctx->offset + entry_size > ctx->buf_size) {

      if (!ctx->offset) {

         /*
          * We haven't "returned" any entries yet and the buffer is too small
          * for our first entry.
          */

         return -EINVAL;
      }

      /* We "returned" at least one entry */
      return (int) ctx->offset;
   }

   ctx->ent.d_ino    = vde->ino;
   ctx->ent.d_off    = (u64) ctx->off;
   ctx->ent.d_reclen = entry_size;
   ctx->ent.d_type   = vfs_type_to_linux_dirent_type(vde->type);

   user_ent = (void *)((char *)ctx->user_dirp + ctx->offset);

   if (copy_to_user(user_ent, &ctx->ent, sizeof(ctx->ent)) < 0)
      return -EFAULT;

   if (copy_to_user(user_ent->d_name, vde->name, vde->name_len) < 0)
      return -EFAULT;

   ctx->offset += entry_size;
   ctx->off++;
   ctx->h->pos++;
   return 0;
}

int vfs_getdents64(fs_handle h, struct linux_dirent64 *user_dirp, u32 buf_size)
{
   NO_TEST_ASSERT(is_preemption_enabled());
   fs_handle_base *hb = (fs_handle_base *) h;
   int rc;

   ASSERT(hb != NULL);
   ASSERT(hb->fs->fsops->getdents);

   vfs_getdents_ctx ctx = {
      .h             = hb,
      .user_dirp     = user_dirp,
      .buf_size      = buf_size,
      .offset        = 0,
      .off           = ctx.h->pos + 1,
      .ent           = { 0 },
   };

   /* See the comment in vfs.h about the "fs-locks" */
   vfs_fs_shlock(hb->fs);
   {
      rc = hb->fs->fsops->getdents(hb, &vfs_getdents_cb, &ctx);

      if (!rc)
         rc = (int) ctx.offset;
   }
   vfs_fs_shunlock(hb->fs);
   return rc;
}
