/*
 * nnio writer
 *
 * Copyright (c) 2016, Wind River Systems, Inc.
 * All rights reserved.
 *
 * See "LICENSE" for license terms.
 *
 * Author:
 *	  Lans Zhang <jia.zhang@windriver.com>
 */

#include <nnio.h>

static void
show_usage(const char *prog)
{
	info_cont("usage: %s <options> <url>\n", prog);
	info_cont("\noptions:\n");
	info_cont("  --help, -h: Print this help information\n");
	info_cont("  --version, -V: Show version number\n");
	info_cont("  --verbose, -v: Show verbose messages\n");
	info_cont("  --quite, -q: Don't show banner information\n");
	info_cont("  --tx-timeout, -t: Set the socket tx timeout\n");
	info_cont("  --linger-timeout, -l: Set the socket linger timeout\n");
	info_cont("  --socket-name, -n: Set the socket name\n");
	info_cont("  --remote-endpoint, -R: <url> argument is remote\n");
	info_cont("  --exit-delay, -e: Delay to exit\n");
	info_cont("\nurl:\n");
	info_cont("  Specify the transport\n");
}

static void
exit_notify(void)
{
	if (nnio_util_verbose()) {
		int err = nn_errno();

		info("nanowrite exiting with %d (%s)\n", err,
		     nn_strerror(err));
	}
}

int
main(int argc, char **argv)
{
	atexit(exit_notify);

	nnio_options_t options = {
		.show_usage = show_usage,
	};

	nnio_options_parse(argc, argv, &options);

	if (!options.quite)
		nnio_show_banner(argv[0]);

	int sock = nnio_socket_open(NN_PUSH, options.tx_timeout,
				    -1, options.socket_name,
				    options.linger_timeout);
	if (sock < 0)
		return -1;

	nnio_error_assert(options.remote_endpoint, "-R option required");

	int rc;
	int ep = nnio_endpoint_add_remote(sock, *options.remote_endpoint);
	if (ep < 0) {
		rc = -1;
		goto err_add_endpoint;
	}

	unsigned int data_len;
	size_t sz = sizeof(data_len);
	rc = nn_getsockopt(sock, NN_SOL_SOCKET, NN_RCVMAXSIZE, &data_len, &sz);
	nnio_error_assert(!rc, "Failed to get NN_RCVMAXSIZE");

	void *data = malloc(data_len);
	nnio_error_assert(data, "Failed to allocate memory");

	ssize_t len = read(STDIN_FILENO, data, data_len);
	if (len < 0) {
		err("Failed to read data from stdin\n");
		rc = -1;
		goto err_read;
	}

	dbg("reading %ld-byte from stdin ...\n", len);

	if (!len) {
		if (nnio_util_verbose())
			info("read stdin EOF\n");

		goto err_read;
	}

	dbg("preparing to send %d-byte to socket ...\n", (int)len);

	len = nnio_socket_tx(sock, data, len);
	if (len < 0) {
		err("Failed to send %ld-byte data to socket\n", len);
		rc = -1;
		goto err_read;
	}

	if (nnio_util_verbose())
		info("Total tx length: %ld-byte\n", len);

err_read:
	free(data);

	/* If the tx socket is closed before the sent data received, the rx
	 * socket would be blocked forever. Essentially speaking, this is
	 * caused by the lack of the support for the linger timeout in
	 * nanomsg.
	 */
	if (options.exit_delay)
		usleep(options.exit_delay);

	dbg("closing socket ...\n");

	nnio_endpoint_delete(sock, ep);

err_add_endpoint:
	nnio_socket_close(sock);

	return rc;
}
