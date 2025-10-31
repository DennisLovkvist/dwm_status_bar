#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define XMR_NODE_IP      "127.0.0.1"
#define XMR_NODE_USER    "delo"
#define XMR_NODE_COMMAND "curl -s http://127.0.0.1:18081/get_info"

int xmr_node_check_status()
{
    ssh_session session = NULL;
    ssh_channel channel = NULL;
    int rc;
    char buffer[1024];
    int nbytes;
    int result = 1; // default: failure

    session = ssh_new();
    if (session == NULL)
    {
        goto cleanup;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, XMR_NODE_IP);
    ssh_options_set(session, SSH_OPTIONS_USER, XMR_NODE_USER);

    rc = ssh_connect(session);
    if (rc != SSH_OK)
    {
        goto cleanup;
    }

    rc = ssh_userauth_publickey_auto(session, NULL, NULL);
    if (rc != SSH_AUTH_SUCCESS)
    {
        goto cleanup;
    }

    channel = ssh_channel_new(session);
    if (channel == NULL)
    {
        goto cleanup;
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        goto cleanup;
    }

    rc = ssh_channel_request_exec(channel, XMR_NODE_COMMAND);
    if (rc != SSH_OK)
    {
        goto cleanup;
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    if (nbytes > 0) 
    {
        buffer[nbytes] = '\0';
        if (strstr(buffer, "\"height\""))
        {
            result = 0;
        }
    }

    cleanup:
    if (channel) 
    {
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    if (session) 
    {
        ssh_disconnect(session);
        ssh_free(session);
    }

    return result;
}
