/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Changes from Qualcomm Innovation Center are provided under the following license:
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#ifndef CLD80211_LIB_H
#define CLD80211_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
#include <netlink/genl/genl.h>
#include <stdbool.h>

#ifndef UNUSED
#define UNUSED(x)    (void)(x)
#endif

/**
 * enum cld80211_attr - Driver/Application embeds the data in nlmsg with the
 *                      help of below attributes
 * CLD80211_ATTR_VENDOR_DATA: Embed all other attributes in this nested
 *                            attribute.
 * CLD80211_ATTR_DATA: Embed driver/application data in this attribute
 * CLD80211_ATTR_META_DATA: Embed meta data for above data. This will  help
 * wlan driver to peek into request message packet without opening up definition
 * of complete request message.
 * @CLD80211_ATTR_CMD: cld80211 vendor subcommand in this attribute
 * @CLD80211_ATTR_CMD_TAG_DATA: cld80211 vendor subcommand data is present in
 * this attribute. It is a nested attribute with sub attributes of specified
 * vendor sub command.
 *
 * Any new message in future can be added as another attribute
 */
enum cld80211_attr {
	CLD80211_ATTR_VENDOR_DATA = 1,
	CLD80211_ATTR_DATA,
	CLD80211_ATTR_META_DATA,
	CLD80211_ATTR_CMD,
	CLD80211_ATTR_CMD_TAG_DATA,

	__CLD80211_ATTR_AFTER_LAST,
	CLD80211_ATTR_MAX = __CLD80211_ATTR_AFTER_LAST - 1
};

/**
 * Create socket of type NETLINK_GENERIC
 * Retuns valid sock only if socket creation is succesful and cld80211
 * family is present, returns NULL otherwise
 */
void *cld80211_init(void);

/**
 * free the socket created in cld80211_init()
 */
void cld80211_deinit(void *cldctx);

/**
 * Allocate nl_msg and populate family and genl header details
 */
struct nl_msg *cld80211_msg_alloc(void *cldctx, int cmd, struct nlattr **nla_data,
				  int pid);

/**
 * Send nlmsg to driver and return; It doesn't wait for response
 */
int cld80211_send_msg(void *cldctx, struct nl_msg *nlmsg);

/**
 * Send nlmsg to driver and get response, if any
 */
int cld80211_send_recv_msg(void *cldctx, struct nl_msg *nlmsg,
			   int (*valid_handler)(struct nl_msg *, void *),
			   void *valid_data);

/**
 * Add membership for multicast group "mcgroup" to receive the messages
 * sent to this group from driver
 */
int cld80211_add_mcast_group(void *cldctx, const char* mcgroup);

/**
 * Remove membership of multicast group "mcgroup" to stop receiving messages
 * sent to this group from driver
 */
int cld80211_remove_mcast_group(void *cldctx, const char* mcgroup);

/**
 * Receive messages from driver on cld80211 family. Client can do
 * a select()/poll() on the socket before calling this API.
 * sock: nl_sock created for communication
 * cb: nl callback context provided by client
 * Returns corresponding errno when a failure happens while receiving nl msg
 */
int cld80211_recv_msg(struct nl_sock *sock, struct nl_cb *cb);

/**
 * Receive messages from driver on cld80211 family from the
 * multicast groups subscribed
 * timeout: Timeout in milliseconds for poll(); -1 is for infinite timeout.
 * recv_multi_msg: Boolean flag to be sent false/true from client to indicate
 *                 whether it wants to receive only one message or multiple
 *                 messages from timeoutblock.
 *                 false: Receive only one message and return
 *                 true: Continue in the loop to receive multiple message till
 *                       client explicitly sends exit via exit_cld80211_recv().
 * cbctx: Context provided by client, which is to be used when an
 *        nlmsg is received
 * Returns corresponding errno when a failure happens while receiving nl msg
 */
int cld80211_recv(void *cldctx, int timeout, bool recv_multi_msg,
		  int (*valid_handler)(struct nl_msg *, void *),
		  void *cbctx);

/**
 * poll() is a blocking call on sock. Client has to unblock the poll()
 * first to exit gracefully.
 */
void exit_cld80211_recv(void *cldctx);

/**
 * Client has to inform to exit gracefully during polling and reset the flag
 * accordingly.
 */
void cld80211_stop_recv(void *cldctx, bool is_terminating);

/**
 * Get the nl socket context to listen to driver events.
 */
struct nl_sock *cld80211_get_nl_socket_ctx(void *cldctx);

/**
 * Get the exit socket pair for clients to stop listening to driver events.
 */
int *cld80211_get_exit_socket_pair(void *cldctx);
#ifdef __cplusplus
}
#endif

#endif
