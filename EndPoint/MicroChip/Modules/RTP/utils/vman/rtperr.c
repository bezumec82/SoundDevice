/*
 * $Id: rtperr.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/ioctl.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>      /* struct sockaddr */
#include <netdb.h>           /* gethostbyname() */
#include <netinet/in.h>      /* sockaddr_in */
#include <arpa/inet.h>       /* inet_addr() */
#include "rtperr.h"

char *RTPStrError(rtperror err)
{
  static char msg[64];
  switch(err){
  case RTP_OK:
    return "No error";
  case RTP_CANT_ALLOC_MEM:
    return "Memory allocation failed";
  case RTP_TOO_MANY_CONTEXTS:
    return "Maximum number of contexts exceeded";
  case RTP_UNKNOWN_CONTEXT:
    return "Context destroyed or never created";
  case RTP_NOSOCKET:
    return "Socket not open for reading or writing";
  case RTP_CANT_GET_SOCKET:
    return "Can't obtain a new socket";
  case RTP_CANT_BIND_SOCKET:
    return "Can't bind socket";
  case RTP_CANT_SET_SOCKOPT:
    return "Unable to set socket option";
  case RTP_SOCKET_WRITE_FAILURE:
    return "Unable to write to socket";
  case RTP_SOCKET_READ_FAILURE:
    return "Unable to read from socket";
  case RTP_SOCKET_MISMATCH:
    return "Socket does not belong to session";
  case RTP_FIXED_WHEN_OPEN:
    return "Change not allowed during open session";
  case RTP_BAD_ADDR:
    return "Invalid address specified";
  case RTP_BAD_PORT:
    return "Invalid port specified";
  case RTP_CANT_CLOSE_SESSION:
    return "Unable to close session";
  case RTP_BAD_MI:
    return "No such membership info";
  case RTP_BAD_PROFILE:
    return "Invalid rtpprofile specified";
  case RTP_SOCKET_EMPTY:
    return "no data to receive at socket";
  case RTP_PACKET_LOOPBACK:
    return "Packet looped back";
  case RTP_INSUFFICIENT_BUFFER:
    return "Allocated buffer too small";
  case RTP_CSRC_LIST_NOT_SET:
    return "CSRC list not yet set";
  case RTP_CSRC_APPEARS_TWICE:
    return "2 CSRCs on list have same SSRC";
  case RTP_BAD_PACKET_FORMAT:
    return "Packet has incorrect format";
  case RTP_NO_SUCH_PERSON:
    return "Person not session member";
  case RTP_PERSON_DATA_PROTECTED:
    return "Person data protected";
  case RTP_NO_SUCH_SENDER:
    return "No such sender in current session";
  case RTP_NO_SUCH_SDES:
    return "No such SDES field";
  case RTP_CANT_USE_ENCRYPTION:
    return "Encryption / decryption function not set";
  case RTP_DECRYPTION_FAILURE:
    return "Packet was unsuccessfully decrypted";
  case RTP_END_OF_LIST:
    return "End of iterating list";
  case RTP_CANT_CALL_FUNCTION:
    return "Not allowed to recurse";
  case RTP_SSRC_IN_USE:
    return "SSRC in use";
  case RTP_CANT_GETSOCKNAME:
    return "Couldn't get socket name";
  case RTP_BAD_VALUE:
    return "Parameter out of range";
  case RTP_NO_SUCH_CONTRIBUTOR:
    return "No such contributor";
  case RTP_CSRC_LIST_FULL:
    return "CSRC list full";
  case RTP_DONT_SEND_NOW:
    return "Don't send this packet now";
  default:
    sprintf(msg, "Unknown RTP Error %d", err);
    return msg;
  }
}
