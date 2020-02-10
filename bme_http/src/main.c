/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"

static const char *url = "http://www.google.com";
static int exit_flag = 0;

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REPLY) {
    struct http_message *hm = (struct http_message *)p;
    c->flags |= MG_F_CLOSE_IMMEDIATELY;
    fwrite(hm->message.p, 1, (int)hm->message.len, stdout);
    putchar('\n');
    exit_flag = 1;
  } else if (ev == MG_EV_CLOSE) {
    exit_flag = 1;
  };
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mg_mgr* mgr = mgos_get_mgr();
  mg_connect_http(mgr, ev_handler, url, NULL, NULL);
  while (exit_flag == 0) {
    mg_mgr_poll(mgr, 1000);
  }
  mg_mgr_free(mgr);
  return MGOS_APP_INIT_SUCCESS;
}
