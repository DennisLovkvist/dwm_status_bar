#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define XMR_NODE_IP "127.0.0.1"
#define XMR_NODE_USER "admin"

int xmr_node_check_status()
{
    ssh_session session;
    ssh_channel channel;
    int rc;
    char buffer[1024];
    int nbytes;

    session = ssh_new();
    if (session == NULL)return 1;
    
    ssh_options_set(session, SSH_OPTIONS_HOST, XMR_NODE_IP);
    ssh_options_set(session, SSH_OPTIONS_USER, XMR_NODE_USER);

    rc = ssh_connect(session);
    if (rc != SSH_OK) 
    {
        ssh_free(session);
        return 1;
    }

    rc = ssh_userauth_publickey_auto(session, NULL, NULL);
    if (rc != SSH_AUTH_SUCCESS) 
    {
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    channel = ssh_channel_new(session);
    if (channel == NULL) return 1;

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) 
    {
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    rc = ssh_channel_request_exec(channel, "curl -s http://127.0.0.1:18081/get_info");
    if (rc != SSH_OK) 
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    if (nbytes > 0) 
    {
        buffer[nbytes] = '\0'; // null-terminate
        if (strstr(buffer, "\"height\"")) 
        {
            return 0;
        } 
        else 
        {
            return 1;
        }
    } 
    else 
    {
        return 1;
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return 0;
}