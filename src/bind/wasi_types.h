//
// Created by Keith on 2020/1/20.
//

#pragma once

#include "../twa.h"

namespace TWA {
namespace WASI {

typedef U32 Wasi_size_t;
typedef U64 Wasi_filesize_t;
typedef U64 Wasi_timestamp_t;

enum Wasi_clockid_t : U32 {
    realtime,
    monotonic,
    process_cputime_id,
    thread_cputime_id
};

enum Wasi_errno_t : U16 {
    success,
    toobig,
    acces,
    addrinuse,
    addrnotavail,
    afnosupport,
    again,
    already,
    badf,
    badmsg,
    busy,
    canceled,
    child,
    connaborted,
    connrefused,
    connreset,
    deadlk,
    destaddrreq,
    dom,
    dquot,
    exist,
    fault,
    fbig,
    hostunreach,
    idrm,
    ilseq,
    inprogress,
    intr,
    inval,
    io,
    isconn,
    isdir,
    loop,
    mfile,
    mlink,
    msgsize,
    multihop,
    nametoolong,
    netdown,
    netreset,
    netunreach,
    nfile,
    nobufs,
    nodev,
    noent,
    noexec,
    nolck,
    nolink,
    nomem,
    nomsg,
    noprotoopt,
    nospc,
    nosys,
    notconn,
    notdir,
    notempty,
    notrecoverable,
    notsock,
    notsup,
    notty,
    nxio,
    overflow,
    ownerdead,
    perm,
    pipe,
    proto,
    protonosupport,
    prototype,
    range,
    rofs,
    spipe,
    srch,
    stale,
    timedout,
    txtbsy,
    xdev,
    notcapable
};

enum Wasi_rights_t : U64 {
    right_fd_datasync = 0x0000000000000001ULL,
    right_fd_read = 0x0000000000000002ULL,
    right_fd_seek = 0x0000000000000004ULL,
    right_fd_fdstat_set_flags = 0x0000000000000008ULL,
    right_fd_sync = 0x0000000000000010ULL,
    right_fd_tell = 0x0000000000000020ULL,
    right_fd_write = 0x0000000000000040ULL,
    right_fd_advise = 0x0000000000000080ULL,
    right_fd_allocate = 0x0000000000000100ULL,
    right_path_create_directory = 0x0000000000000200ULL,
    right_path_create_file = 0x0000000000000400ULL,
    right_path_link_source = 0x0000000000000800ULL,
    right_path_link_target = 0x0000000000001000ULL,
    right_path_open = 0x0000000000002000ULL,
    right_fd_readdir = 0x0000000000004000ULL,
    right_path_readlink = 0x0000000000008000ULL,
    right_path_rename_source = 0x0000000000010000ULL,
    right_path_rename_target = 0x0000000000020000ULL,
    right_path_filestat_get = 0x0000000000040000ULL,
    right_path_filestat_set_size = 0x0000000000080000ULL,
    right_path_filestat_set_times = 0x0000000000100000ULL,
    right_fd_filestat_get = 0x0000000000200000ULL,
    right_fd_filestat_set_size = 0x0000000000400000ULL,
    right_fd_filestat_set_times = 0x0000000000800000ULL,
    right_path_symlink = 0x0000000001000000ULL,
    right_path_remove_directory = 0x0000000002000000ULL,
    right_path_unlink_file = 0x0000000004000000ULL,
    right_poll_fd_readwrite = 0x0000000008000000ULL,
    right_sock_shutdown = 0x0000000010000000ULL
};

typedef U32 Wasi_fd_t;

struct Wasi_iovec_t {
    const U8 *buf;
    Wasi_size_t buf_len;
};

typedef I64 Wasi_filedelta_t;

enum Wasi_whence_t : U8 {
    set,
    cur,
    end
};

typedef U64 Wasi_dircookie_t;
typedef U32 Wasi_dirnamlen_t;
typedef U64 Wasi_inode_t;

enum Wasi_filetype_t : U8 {
    unknown,
    block_device,
    character_device,
    directory,
    regular_file,
    socket_dgram,
    socket_stream,
    symbolic_link
};

struct Wasi_dirent_t {
    Wasi_dircookie_t d_next;
    Wasi_inode_t d_ino;
    Wasi_dirnamlen_t d_namlen;
    Wasi_filetype_t d_type;
};

enum Wasi_advice_t : U8 {
    normal,
    sequential,
    random,
    willneed,
    dontneed,
    noreuse
};

enum Wasi_fdflags_t : U16 {
    fdflag_append = 0x0001,
    fdflag_dsync = 0x0002,
    fdflag_nonblock = 0x0004,
    fdflag_rsync = 0x0008,
    fdflag_sync = 0x0010
};

struct Wasi_fdstat_t {
    Wasi_filetype_t fs_filetype;
    Wasi_fdflags_t fs_flags;
    Wasi_rights_t fs_rights_base;
    Wasi_rights_t fs_rights_inheriting;
};

typedef U64 Wasi_device_t;

enum Wasi_fstflags_t : U16 {
    fstatim = 0x0001,
    fstatim_now = 0x0002,
    fstmtim = 0x0004,
    fstmtim_now = 0x0008
};

enum Wasi_lookupflags_t : U32 {
    lookup_symlink_follow = 0x00000001U
};

enum Wasi_oflags_t : U16 {
    o_creat = 0x0001,
    o_directory_ = 0x0002,
    o_excl = 0x0004,
    o_trunc = 0x0008
};

typedef U64 Wasi_linkcount_t;

struct Wasi_filestat_t {
    Wasi_device_t dev;
    Wasi_inode_t ino;
    Wasi_filetype_t filetype;
    Wasi_linkcount_t nlink;
    Wasi_filesize_t size;
    Wasi_timestamp_t atim;
    Wasi_timestamp_t mtim;
    Wasi_timestamp_t ctim;
};

typedef U64 Wasi_userdata_t;
enum Wasi_eventtype_t : U8 {
    clock,
    fd_read,
    fd_write
};

enum Wasi_eventrwflags_t : U16 {
    fd_readwrite_hangup
};

struct Wasi_event_fd_readwrite_t {
    Wasi_filesize_t nbytes;
    Wasi_eventrwflags_t flags;
};

union Wasi_event_u_t {
    Wasi_event_fd_readwrite_t fd_readwrite;
};

struct Wasi_event_t {
    Wasi_userdata_t userdata;
    Wasi_errno_t error;
    Wasi_eventtype_t type;
    Wasi_event_u_t u;
};

enum Wasi_subclockflags_t : U16 {
    subscription_clock_abstime = 0x0001
};

struct Wasi_subscription_clock_t {
    Wasi_clockid_t id;
    Wasi_timestamp_t timeout;
    Wasi_timestamp_t precision;
    Wasi_subclockflags_t flags;
};

struct Wasi_subscription_fd_readwrite_t {
    Wasi_fd_t file_descriptor;
};

union Wasi_subscription_u_t {
    Wasi_subscription_clock_t clock;
    Wasi_subscription_fd_readwrite_t fd_readwrite;
};

struct Wasi_subscription_t {
    Wasi_userdata_t userdata;
    Wasi_eventtype_t type;
    Wasi_subscription_u_t u;
};

typedef U32 Wasi_exitcode_t;

enum Wasi_signal_t : U8 {
    signone,
    sighup,
    sigint,
    sigquit,
    sigill,
    sigtrap,
    sigabrt,
    sigbus,
    sigfpe,
    sigkill,
    sigusr1,
    sigsegv,
    sigusr2,
    sigpipe,
    sigalrm,
    sigterm,
    sigchld,
    sigcont,
    sigstop,
    sigtstp,
    sigttin,
    sigttou,
    sigurg,
    sigxcpu,
    sigxfsz,
    sigvtalrm,
    sigprof,
    sigwinch,
    sigpoll,
    sigpwr,
    sigsys
};

enum Wasi_riflags_t : U16 {
    ri_recv_peek = 0x0001,
    ri_recv_waitall = 0x0002
};

enum Wasi_roflags_t : U16 {
    sock_recv_data_truncated = 0x0001
};

typedef U16 Wasi_siflags_t;

enum Wasi_sdflags_t : U8 {
    sd_rd = 0x01,
    sd_wr = 0x02
};

enum Wasi_preopentype_t : U8 {
    dir
};

struct Wasi_prestat_dir_t {
    Wasi_size_t pr_name_len;
};

union Wasi_prestat_u_t {
    Wasi_prestat_dir_t dir;
};

struct Wasi_prestat_t {
    Wasi_preopentype_t pr_type;
    Wasi_prestat_u_t u;
};

}
}
