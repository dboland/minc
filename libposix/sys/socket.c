/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <sys/socket.h>

/****************************************************/

void 
sflags_debug(int flags, const char *lable)
{
	DWORD dwRemain = flags;

	msvc_printf("  %s(0x%x): ", lable, dwRemain);
	win_flagname(MSG_OOB, "OOB", dwRemain, &dwRemain);
	win_flagname(MSG_DONTROUTE, "DONTROUTE", dwRemain, &dwRemain);
	win_flagname(MSG_NOSIGNAL, "NOSIGNAL", dwRemain, &dwRemain);
	win_flagname(MSG_DONTWAIT, "DONTWAIT", dwRemain, &dwRemain);

	msvc_printf(" remain(0x%x)\n", dwRemain);
}
void 
addr_debug(const struct sockaddr *addr, socklen_t addrlen, const char *label)
{
	int i = 0;
	int len = 14;	/* default for struct sockaddr */

	if (!addr){
		return;
	}
	msvc_printf("%s(): addrlen(%02d) sa_family(%d) sa_len(%02d) sa_data(", 
		label, addrlen, addr->sa_family, addr->sa_len);
	while (i < len){
		msvc_printf("[%d]", (unsigned char)addr->sa_data[i++]);
	}
	msvc_printf(")\n");
}

/****************************************************/

DWORD 
af_win(int family)
{
	DWORD dwResult = family;

	if (family == AF_ROUTE){
		dwResult = WIN_AF_ROUTE;
	}else if (family == AF_INET6){
		dwResult = WIN_AF_INET6;
	}
	return(dwResult);
}
int 
af_posix(DWORD Family)
{
	int result = Family;

	if (Family == WIN_AF_ROUTE){
		result = AF_ROUTE;
	}else if (Family == WIN_AF_INET6){
		result = AF_INET6;
	}
	return(result);
}
SOCKADDR *
addr_win(SOCKADDR *Result, WIN_TASK *Task, const struct sockaddr *addr)
{
	WIN_NAMEIDATA wPath;

	if (!addr){
		return(NULL);
	}
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "sockaddr", 8, addr, addr->sa_len);
	}
	if (addr->sa_family == AF_LOCAL){
		path_win(&wPath, addr->sa_data, 0);
		win_wcsncpy((LPWSTR)Result->sa_data, wPath.Resolved, MAX_PATH);
	}else{
		/* no more than struct sockaddr default */
		win_memcpy(Result->sa_data, addr->sa_data, 14);
	}
	Result->sa_family = af_win(addr->sa_family);
	return(Result);
}
struct sockaddr *
addr_posix(struct sockaddr *result, socklen_t *addrlen, WIN_TASK *Task, SOCKADDR *Address)
{
	if (!result){
		return(NULL);
	}
	if (Address->sa_family == WIN_AF_LOCAL){
		path_posix(result->sa_data, (LPWSTR)Address->sa_data);
	}else{
		/* no more than struct sockaddr default */
		win_memcpy(result->sa_data, Address->sa_data, 14);
	}
	result->sa_family = af_posix(Address->sa_family);
	result->sa_len = (__uint8_t)(*addrlen);
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "sockaddr", 8, result, result->sa_len);
	}
	return(result);
}
LPWSABUF
iovec_win(struct iovec vec[], unsigned int size)
{
	size_t len;
	void *base;
	LPWSABUF result = (LPWSABUF)vec;

	/* WinSock's message header (WSAMSG) is 99% compatible.
	 * Only the two fields in its iovec structure (WSABUF) 
	 * need to be reversed.
	 */
	while (size--){
		len = vec->iov_len;
		base = vec->iov_base;
		vec->iov_len = (size_t)base;
		vec->iov_base = (void *)len;
		vec++;
	}
	return(result);
}
WSAMSG *
msghdr_win(WSAMSG *Result, const struct msghdr *msg)
{
	Result->lpBuffers = iovec_win(msg->msg_iov, msg->msg_iovlen);
	Result->dwBufferCount = msg->msg_iovlen;
	Result->Control.buf = msg->msg_control;
	Result->Control.len = msg->msg_controllen;
	Result->dwFlags = msg->msg_flags;
	return(Result);
}
void 
cmsg_win(WIN_TASK *Task, struct cmsghdr *cmsg)
{
	int fd;
	HANDLE hChannel;
	WIN_VNODE vNode = {0};
	unsigned char *data = CMSG_DATA(cmsg);

	switch (cmsg->cmsg_type){
		case WIN_SCM_RIGHTS:
			fd = *(int *)data;
			vfs_F_DUPFD(&Task->Node[fd], FALSE, &vNode);
			*(HANDLE *)data = vNode.Handle;
			cmsg->cmsg_type = WIN_SCM_ACCESS;
			break;
		case WIN_SCM_ACCESS:
			hChannel = *(HANDLE *)data;
			SetLastError(ERROR_SUCCESS);	/* yes. */
			vfs_namei(hChannel, 1, &vNode);
			*(int *)data = fd_posix(Task, &vNode, 0);
			cmsg->cmsg_type = WIN_SCM_RIGHTS;
			break;
	}
//__PRINTF("  cmsg_win(%d): len(%d) level(0x%x) type(0x%x) data(%d)\n", 
//	task->pid, cmsg->cmsg_len, cmsg->cmsg_level, cmsg->cmsg_type, *(int *)data)
}
void 
cmsghdr_win(WIN_TASK *Task, const struct msghdr *msg)
{
	struct cmsghdr *cmsg = NULL;

	if (cmsg = CMSG_FIRSTHDR(msg)){
		do {
			cmsg_win(Task, cmsg);		/* syslogd.exe */
		} while (cmsg = CMSG_NXTHDR(msg, cmsg));
	}
}

/****************************************************/

int 
sys_getrtable(call_t call)
{
	return(call.Task->SessionId);
}
int 
sys_socket(call_t call, int family, int type, int protocol)
{
	int result = 0;
	WIN_VNODE vNode = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_socket(af_win(family), type, protocol, &vNode)){
		result -= errno_posix(WSAGetLastError());
	}else{
		result = fd_posix(pwTask, &vNode, 0);
	}
	return(result);
}
int 
sys_connect(call_t call, int sockfd, const struct sockaddr *address, socklen_t addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* ab.exe */
		result = -EBADF;
	}else if (!vfs_connect(&pwTask->Node[sockfd], addr_win(&sAddress, pwTask, address), addrlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_bind(call_t call, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* sshd.exe */
		result = -EBADF;
	}else if (!vfs_bind(&pwTask->Node[sockfd], addr_win(&sAddress, pwTask, addr), addrlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_accept(call_t call, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_VNODE vnResult = {0};
	WIN_TASK *pwTask = call.Task;

	pwTask->State = SSLEEP;
	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* ab.exe */
		result = -EBADF;
	}else if (!vfs_accept(&pwTask->Node[sockfd], &sAddress, addrlen, &vnResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		addr_posix(addr, addrlen, pwTask, &sAddress);
		result = fd_posix(pwTask, &vnResult, 0);
	}
	pwTask->State = SRUN;
	return(result);
}
ssize_t 
sys_recvfrom(call_t call, int sockfd, void *buf, size_t size, int flags, 
	struct sockaddr *src_addr, socklen_t *addrlen)
{
	ssize_t result = 0;
	DWORD dwResult = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_recvfrom(&pwTask->Node[sockfd], buf, size, flags, addr_win(&sAddress, pwTask, src_addr), addrlen, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		addr_posix(src_addr, addrlen, pwTask, &sAddress);
		if (pwTask->TracePoints & KTRFAC_GENIO){
			ktrace_GENIO(pwTask, sockfd, UIO_READ, buf, dwResult);
		}
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_sendto(call_t call, int sockfd, const void *buf, size_t size, int flags, 
	const struct sockaddr *dest_addr, socklen_t addrlen)
{
	ssize_t result = 0;
	DWORD dwResult = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, sockfd, UIO_WRITE, buf, size);
	}
//sflags_debug(flags, "sys_sendto");
	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* sshd.exe */
		result = -EBADF;
	}else if (!vfs_sendto(&pwTask->Node[sockfd], buf, size, flags, addr_win(&sAddress, pwTask, dest_addr), addrlen, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_sendmsg(call_t call, int sockfd, const struct msghdr *msg, int flags)
{
	ssize_t result = 0;
	WSAMSG wMessage = {0};
	SOCKADDR sAddress = {0};
	DWORD dwResult;
	WIN_TASK *pwTask = call.Task;
	DWORD dwFlags = flags;

	wMessage.name = addr_win(&sAddress, pwTask, msg->msg_name);
	wMessage.namelen = msg->msg_namelen;
	cmsghdr_win(pwTask, msg);
	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_sendmsg(&pwTask->Node[sockfd], msghdr_win(&wMessage, msg), dwFlags, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		/* swap back iovec pointers */
		iovec_win(msg->msg_iov, msg->msg_iovlen);
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_recvmsg(call_t call, int sockfd, struct msghdr *msg, int flags)
{
	ssize_t result = 0;
	SOCKADDR sAddress = {0};
	WSAMSG wMessage = {0};
	DWORD dwResult;
	WIN_TASK *pwTask = call.Task;
	DWORD dwFlags = flags;

	wMessage.name = &sAddress;
	wMessage.namelen = msg->msg_namelen;	/* used for in/out (dig.exe) */
	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_recvmsg(&pwTask->Node[sockfd], msghdr_win(&wMessage, msg), &dwFlags, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		/* swap back iovec pointers */
		iovec_win(msg->msg_iov, msg->msg_iovlen);
		addr_posix(msg->msg_name, &msg->msg_namelen, pwTask, wMessage.name);
		cmsghdr_win(pwTask, msg);
		result = dwResult;
	}
	return(result);
}
int 
sys_socketpair(call_t call, int domain, int type, int protocol, int sv[2])
{
	int result = 0;
	WIN_VNODE vnResult[2] = {0};
	WIN_TASK *pwTask = call.Task;
	DWORD dwAttribs = PIPE_READMODE_BYTE;

	if (type == SOCK_DGRAM){
		dwAttribs = PIPE_READMODE_MESSAGE;
	}
	if (!vfs_socketpair(af_win(domain), dwAttribs, protocol, vnResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		sv[0] = fd_posix(pwTask, &vnResult[0], 0);
		sv[1] = fd_posix(pwTask, &vnResult[1], 0);
		if (pwTask->TracePoints & KTRFAC_STRUCT){
			ktrace_STRUCT(pwTask, "fdvec", 5, sv, (sizeof(int) * 2));
		}
	}
	return(result);
}
int 
sys_shutdown(call_t call, int sockfd, int how)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_shutdown(&pwTask->Node[sockfd], how)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_getsockopt(call_t call, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getsockopt(&pwTask->Node[sockfd], level, optname, optval, optlen)){
		result -= errno_posix(WSAGetLastError());
	}else if (level == SOL_SOCKET && optname == SO_ERROR){	// perl.exe
		*(int *)optval = errno_posix(*(DWORD *)optval);
	}
	return(result);
}
int 
sys_setsockopt(call_t call, int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_setsockopt(&pwTask->Node[sockfd], level, optname, optval, optlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_listen(call_t call, int sockfd, int backlog)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_listen(&pwTask->Node[sockfd], backlog)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_getpeername(call_t call, int sockfd, struct sockaddr *address, socklen_t *address_len)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getpeername(&pwTask->Node[sockfd], &sAddress, address_len)){
		result -= errno_posix(WSAGetLastError());
	}else{
		addr_posix(address, address_len, pwTask, &sAddress);
	}
	return(result);
}
int 
sys_getsockname(call_t call, int sockfd, struct sockaddr *address, socklen_t *address_len)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getsockname(&pwTask->Node[sockfd], &sAddress, address_len)){
		result -= errno_posix(WSAGetLastError());
	}else{
		addr_posix(address, address_len, pwTask, &sAddress);
	}
	return(result);
}
