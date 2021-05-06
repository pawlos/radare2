/*
 * sfsysnr.h --- SFLib syscall numbers for FreeBSD/i386 
 * see http://www.secdev.org/projects/shellforge.html for more informations
 *
 * Copyright (C) 2004  Philippe Biondi <phil@secdev.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */


/*
 * Automatically generated by gensflib.py 
 * Wed Mar 31 17:50:12 2021
 */

#ifndef SFSYSNR_H
#define SFSYSNR_H
        
/* $Id$ */

/* From /usr/include/sys/syscall.h */ 

#define	__NR_syscall	0
#define	__NR_exit	1
#define	__NR_fork	2
#define	__NR_read	3
#define	__NR_write	4
#define	__NR_open	5
#define	__NR_close	6
#define	__NR_wait4	7
				/* 8 is old creat */
#define	__NR_link	9
#define	__NR_unlink	10
				/* 11 is obsolete execv */
#define	__NR_chdir	12
#define	__NR_fchdir	13
#define	__NR_mknod	14
#define	__NR_chmod	15
#define	__NR_chown	16
#define	__NR_break	17
#define	__NR_getfsstat	18
				/* 19 is old lseek */
#define	__NR_getpid	20
#define	__NR_mount	21
#define	__NR_unmount	22
#define	__NR_setuid	23
#define	__NR_getuid	24
#define	__NR_geteuid	25
#define	__NR_ptrace	26
#define	__NR_recvmsg	27
#define	__NR_sendmsg	28
#define	__NR_recvfrom	29
#define	__NR_accept	30
#define	__NR_getpeername	31
#define	__NR_getsockname	32
#define	__NR_access	33
#define	__NR_chflags	34
#define	__NR_fchflags	35
#define	__NR_sync	36
#define	__NR_kill	37
				/* 38 is old stat */
#define	__NR_getppid	39
				/* 40 is old lstat */
#define	__NR_dup	41
#define	__NR_pipe	42
#define	__NR_getegid	43
#define	__NR_profil	44
#define	__NR_ktrace	45
				/* 46 is old sigaction */
#define	__NR_getgid	47
				/* 48 is old sigprocmask */
#define	__NR_getlogin	49
#define	__NR_setlogin	50
#define	__NR_acct	51
				/* 52 is old sigpending */
#define	__NR_sigaltstack	53
#define	__NR_ioctl	54
#define	__NR_reboot	55
#define	__NR_revoke	56
#define	__NR_symlink	57
#define	__NR_readlink	58
#define	__NR_execve	59
#define	__NR_umask	60
#define	__NR_chroot	61
				/* 62 is old fstat */
				/* 63 is old getkerninfo */
				/* 64 is old getpagesize */
#define	__NR_msync	65
#define	__NR_vfork	66
				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
#define	__NR_sbrk	69
#define	__NR_sstk	70
				/* 71 is old mmap */
#define	__NR_vadvise	72
#define	__NR_munmap	73
#define	__NR_mprotect	74
#define	__NR_madvise	75
				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
#define	__NR_mincore	78
#define	__NR_getgroups	79
#define	__NR_setgroups	80
#define	__NR_getpgrp	81
#define	__NR_setpgid	82
#define	__NR_setitimer	83
				/* 84 is old wait */
#define	__NR_swapon	85
#define	__NR_getitimer	86
				/* 87 is old gethostname */
				/* 88 is old sethostname */
#define	__NR_getdtablesize	89
#define	__NR_dup2	90
#define	__NR_fcntl	92
#define	__NR_select	93
#define	__NR_fsync	95
#define	__NR_setpriority	96
#define	__NR_socket	97
#define	__NR_connect	98
				/* 99 is old accept */
#define	__NR_getpriority	100
				/* 101 is old send */
				/* 102 is old recv */
				/* 103 is old sigreturn */
#define	__NR_bind	104
#define	__NR_setsockopt	105
#define	__NR_listen	106
				/* 107 is obsolete vtimes */
				/* 108 is old sigvec */
				/* 109 is old sigblock */
				/* 110 is old sigsetmask */
				/* 111 is old sigsuspend */
				/* 112 is old sigstack */
				/* 113 is old recvmsg */
				/* 114 is old sendmsg */
				/* 115 is obsolete vtrace */
#define	__NR_gettimeofday	116
#define	__NR_getrusage	117
#define	__NR_getsockopt	118
#define	__NR_readv	120
#define	__NR_writev	121
#define	__NR_settimeofday	122
#define	__NR_fchown	123
#define	__NR_fchmod	124
				/* 125 is old recvfrom */
#define	__NR_setreuid	126
#define	__NR_setregid	127
#define	__NR_rename	128
				/* 129 is old truncate */
				/* 130 is old ftruncate */
#define	__NR_flock	131
#define	__NR_mkfifo	132
#define	__NR_sendto	133
#define	__NR_shutdown	134
#define	__NR_socketpair	135
#define	__NR_mkdir	136
#define	__NR_rmdir	137
#define	__NR_utimes	138
				/* 139 is obsolete 4.2 sigreturn */
#define	__NR_adjtime	140
				/* 141 is old getpeername */
				/* 142 is old gethostid */
				/* 143 is old sethostid */
				/* 144 is old getrlimit */
				/* 145 is old setrlimit */
				/* 146 is old killpg */
#define	__NR_setsid	147
#define	__NR_quotactl	148
				/* 149 is old quota */
				/* 150 is old getsockname */
#define	__NR_nfssvc	155
				/* 156 is old getdirentries */
#define	__NR_statfs	157
#define	__NR_fstatfs	158
#define	__NR_getfh	161
#define	__NR_getdomainname	162
#define	__NR_setdomainname	163
#define	__NR_uname	164
#define	__NR_sysarch	165
#define	__NR_rtprio	166
#define	__NR_semsys	169
#define	__NR_msgsys	170
#define	__NR_shmsys	171
#define	__NR_pread	173
#define	__NR_pwrite	174
#define	__NR_ntp_adjtime	176
#define	__NR_setgid	181
#define	__NR_setegid	182
#define	__NR_seteuid	183
#define	__NR_stat	188
#define	__NR_fstat	189
#define	__NR_lstat	190
#define	__NR_pathconf	191
#define	__NR_fpathconf	192
#define	__NR_getrlimit	194
#define	__NR_setrlimit	195
#define	__NR_getdirentries	196
#define	__NR_mmap	197
#define	__NR___syscall	198
#define	__NR_lseek	199
#define	__NR_truncate	200
#define	__NR_ftruncate	201
#define	__NR___sysctl	202
#define	__NR_mlock	203
#define	__NR_munlock	204
#define	__NR_undelete	205
#define	__NR_futimes	206
#define	__NR_getpgid	207
#define	__NR_poll	209
#define	__NR___semctl	220
#define	__NR_semget	221
#define	__NR_semop	222
#define	__NR_msgctl	224
#define	__NR_msgget	225
#define	__NR_msgsnd	226
#define	__NR_msgrcv	227
#define	__NR_shmat	228
#define	__NR_shmctl	229
#define	__NR_shmdt	230
#define	__NR_shmget	231
#define	__NR_clock_gettime	232
#define	__NR_clock_settime	233
#define	__NR_clock_getres	234
#define	__NR_nanosleep	240
#define	__NR_minherit	250
#define	__NR_rfork	251
#define	__NR_openbsd_poll	252
#define	__NR_issetugid	253
#define	__NR_lchown	254
#define	__NR_getdents	272
#define	__NR_lchmod	274
#define	__NR_netbsd_lchown	275
#define	__NR_lutimes	276
#define	__NR_netbsd_msync	277
#define	__NR_nstat	278
#define	__NR_nfstat	279
#define	__NR_nlstat	280
#define	__NR_fhstatfs	297
#define	__NR_fhopen	298
#define	__NR_fhstat	299
#define	__NR_modnext	300
#define	__NR_modstat	301
#define	__NR_modfnext	302
#define	__NR_modfind	303
#define	__NR_kldload	304
#define	__NR_kldunload	305
#define	__NR_kldfind	306
#define	__NR_kldnext	307
#define	__NR_kldstat	308
#define	__NR_kldfirstmod	309
#define	__NR_getsid	310
#define	__NR_setresuid	311
#define	__NR_setresgid	312
				/* 313 is obsolete signanosleep */
#define	__NR_aio_return	314
#define	__NR_aio_suspend	315
#define	__NR_aio_cancel	316
#define	__NR_aio_error	317
#define	__NR_aio_read	318
#define	__NR_aio_write	319
#define	__NR_lio_listio	320
#define	__NR_yield	321
				/* 322 is obsolete thr_sleep */
				/* 323 is obsolete thr_wakeup */
#define	__NR_mlockall	324
#define	__NR_munlockall	325
#define	__NR___getcwd	326
#define	__NR_sched_setparam	327
#define	__NR_sched_getparam	328
#define	__NR_sched_setscheduler	329
#define	__NR_sched_getscheduler	330
#define	__NR_sched_yield	331
#define	__NR_sched_get_priority_max	332
#define	__NR_sched_get_priority_min	333
#define	__NR_sched_rr_get_interval	334
#define	__NR_utrace	335
				/* 336 is old sendfile */
#define	__NR_kldsym	337
#define	__NR_jail	338
#define	__NR_sigprocmask	340
#define	__NR_sigsuspend	341
				/* 342 is old sigaction */
#define	__NR_sigpending	343
				/* 344 is old sigreturn */
#define	__NR_sigtimedwait	345
#define	__NR_sigwaitinfo	346
#define	__NR___acl_get_file	347
#define	__NR___acl_set_file	348
#define	__NR___acl_get_fd	349
#define	__NR___acl_set_fd	350
#define	__NR___acl_delete_file	351
#define	__NR___acl_delete_fd	352
#define	__NR___acl_aclcheck_file	353
#define	__NR___acl_aclcheck_fd	354
#define	__NR_extattrctl	355
#define	__NR_extattr_set_file	356
#define	__NR_extattr_get_file	357
#define	__NR_extattr_delete_file	358
#define	__NR_aio_waitcomplete	359
#define	__NR_getresuid	360
#define	__NR_getresgid	361
#define	__NR_kqueue	362
#define	__NR_kevent	363
#define	__NR_extattr_set_fd	371
#define	__NR_extattr_get_fd	372
#define	__NR_extattr_delete_fd	373
#define	__NR___setugid	374
#define	__NR_nfsclnt	375
#define	__NR_eaccess	376
#define	__NR_nmount	378
#define	__NR_kse_exit	379
#define	__NR_kse_wakeup	380
#define	__NR_kse_create	381
#define	__NR_kse_thr_interrupt	382
#define	__NR_kse_release	383
#define	__NR___mac_get_proc	384
#define	__NR___mac_set_proc	385
#define	__NR___mac_get_fd	386
#define	__NR___mac_get_file	387
#define	__NR___mac_set_fd	388
#define	__NR___mac_set_file	389
#define	__NR_kenv	390
#define	__NR_lchflags	391
#define	__NR_uuidgen	392
#define	__NR_sendfile	393
#define	__NR_mac_syscall	394
#define	__NR_ksem_close	400
#define	__NR_ksem_post	401
#define	__NR_ksem_wait	402
#define	__NR_ksem_trywait	403
#define	__NR_ksem_init	404
#define	__NR_ksem_open	405
#define	__NR_ksem_unlink	406
#define	__NR_ksem_getvalue	407
#define	__NR_ksem_destroy	408
#define	__NR___mac_get_pid	409
#define	__NR___mac_get_link	410
#define	__NR___mac_set_link	411
#define	__NR_extattr_set_link	412
#define	__NR_extattr_get_link	413
#define	__NR_extattr_delete_link	414
#define	__NR___mac_execve	415
#define	__NR_sigaction	416
#define	__NR_sigreturn	417
#define	__NR_getcontext	421
#define	__NR_setcontext	422
#define	__NR_swapcontext	423
#define	__NR_swapoff	424
#define	__NR___acl_get_link	425
#define	__NR___acl_set_link	426
#define	__NR___acl_delete_link	427
#define	__NR___acl_aclcheck_link	428
#define	__NR_sigwait	429
#define	__NR_thr_create	430
#define	__NR_thr_exit	431
#define	__NR_thr_self	432
#define	__NR_thr_kill	433
#define	__NR__umtx_lock	434
#define	__NR__umtx_unlock	435
#define	__NR_jail_attach	436
#define	__NR_MAXSYSCALL	437

#endif /* SFSYSNR_H */
