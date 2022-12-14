/*
 * Copyright Runtime.io 2018. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/byteorder.h>
#include "net/buf.h"
#include "mgmt/mcumgr/buf.h"

NET_BUF_POOL_DEFINE(pkt_pool, CONFIG_MCUMGR_BUF_COUNT, CONFIG_MCUMGR_BUF_SIZE,
		    CONFIG_MCUMGR_BUF_USER_DATA_SIZE, NULL);

struct net_buf *
mcumgr_buf_alloc(void)
{
	return net_buf_alloc(&pkt_pool, K_NO_WAIT);
}

void
mcumgr_buf_free(struct net_buf *nb)
{
	net_buf_unref(nb);
}

static uint8_t
cbor_nb_reader_get8(struct cbor_decoder_reader *d, int offset)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset >= cnr->nb->len) {
		return UINT8_MAX;
	}

	return cnr->nb->data[offset];
}

static uint16_t
cbor_nb_reader_get16(struct cbor_decoder_reader *d, int offset)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset > cnr->nb->len - (int)sizeof(uint16_t)) {
		return UINT16_MAX;
	}

	return sys_get_be16(cnr->nb->data + offset);
}

static uint32_t
cbor_nb_reader_get32(struct cbor_decoder_reader *d, int offset)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset > cnr->nb->len - (int)sizeof(uint32_t)) {
		return UINT32_MAX;
	}

	return sys_get_be32(cnr->nb->data + offset);
}

static uint64_t
cbor_nb_reader_get64(struct cbor_decoder_reader *d, int offset)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset > cnr->nb->len - (int)sizeof(uint64_t)) {
		return UINT64_MAX;
	}

	return sys_get_be64(cnr->nb->data + offset);
}

static uintptr_t
cbor_nb_reader_cmp(struct cbor_decoder_reader *d, char *buf, int offset,
		   size_t len)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset > cnr->nb->len - (int)len) {
		return -1;
	}

	return memcmp(cnr->nb->data + offset, buf, len);
}

static uintptr_t
cbor_nb_reader_cpy(struct cbor_decoder_reader *d, char *dst, int offset,
		   size_t len)
{
	struct cbor_nb_reader *cnr;

	cnr = (struct cbor_nb_reader *) d;

	if (offset < 0 || offset > cnr->nb->len - (int)len) {
		return -1;
	}

	return (uintptr_t)memcpy(dst, cnr->nb->data + offset, len);
}

void
cbor_nb_reader_init(struct cbor_nb_reader *cnr,
		    struct net_buf *nb)
{
	cnr->r.get8 = &cbor_nb_reader_get8;
	cnr->r.get16 = &cbor_nb_reader_get16;
	cnr->r.get32 = &cbor_nb_reader_get32;
	cnr->r.get64 = &cbor_nb_reader_get64;
	cnr->r.cmp = &cbor_nb_reader_cmp;
	cnr->r.cpy = &cbor_nb_reader_cpy;

	cnr->nb = nb;
	cnr->r.message_size = nb->len;
}

static int
cbor_nb_write(struct cbor_encoder_writer *writer, const char *data, int len)
{
	struct cbor_nb_writer *cnw;

	cnw = (struct cbor_nb_writer *) writer;
	if (len > net_buf_tailroom(cnw->nb)) {
		return CborErrorOutOfMemory;
	}

	net_buf_add_mem(cnw->nb, data, len);
	cnw->enc.bytes_written += len;

	return CborNoError;
}

void
cbor_nb_writer_init(struct cbor_nb_writer *cnw, struct net_buf *nb)
{
	cnw->nb = nb;
	cnw->enc.bytes_written = 0;
	cnw->enc.write = &cbor_nb_write;
}
