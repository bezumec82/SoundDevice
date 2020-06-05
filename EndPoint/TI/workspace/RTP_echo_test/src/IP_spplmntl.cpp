#include "IP_spplmntl.h"

void Get_IP_addr(
        const char * iface_name,
        char * ret_val
        ) {
    struct ifaddrs *    ifaddr;
    struct ifaddrs *    ifa;
    int32_t             family;
    int32_t             retval;
    int32_t             n;
    char                IP_addr[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        FPRINTF;
        exit(EXIT_FAILURE);
    }

    /* Walk through linked list, maintaining head pointer so we can free list later */
    for (ifa = ifaddr, n = 0;
            ifa != NULL;
            ifa = ifa->ifa_next, n++) {

        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;
        /* For an AF_INET* interface address, display the address */
        if (family == AF_INET) {
            retval = getnameinfo(ifa->ifa_addr,
                    sizeof(struct sockaddr_in),
                    IP_addr,
                    NI_MAXHOST,
                    NULL,
                    0,
                    NI_NUMERICHOST
                    );
            if (retval != 0) {
                PRINT_ERR("'getnameinfo' failed: %s\n", gai_strerror(retval));
                return;
            }
#if DEBUG_IP
            PRINTF(GRN, "IP address of '%-5s' : %s.\r\n", ifa->ifa_name, IP_addr);
#endif
            if  (
            !memcmp (
                    iface_name,
                    ifa->ifa_name,
                    strlen(ifa->ifa_name)
                    )
                ) {
                freeifaddrs(ifaddr);
                strcpy(ret_val, IP_addr);
                return;
            }
        } //end if (family == AF_INET)
    } //end for ...

    PRINT_ERR("IP address for device '%s' didn't found.\r\n", iface_name);
    freeifaddrs(ifaddr);
    return; /* IP address didn't found */
}
/*----------------------------------------------------------------------------*/
