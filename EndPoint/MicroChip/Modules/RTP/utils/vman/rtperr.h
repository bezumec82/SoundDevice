#ifndef __RTP_ERR_H
#define __RTP_ERR_H


/* For error codes, we are making use of elemedia standard
codes:

Format of return codes for all APIs

   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
  +---+-----------+---------------+-------------------------------+
  |Sev| Facility  | Fac-Sub-Code  |               Code            |
 +---+-----------+---------------+-------------------------------+

  where

      Sev - is the severity code (2 bits)

	  00 - Success
	  01 - Informational
	  10 - Warning
	  11 - Error

      R - is a reserved bit   (2 bits)

      Facility - is the facility code ( 6 bits)

	      Fac-Sub-Code - is the facility sub code.

      Code - is the facility's status code (16 bits)

      */


#define EM_MASK(BITS,SHIFT)              (((1U << (BITS)) - 1) << (SHIFT))

#define EM_CODE_SHIFT                    0U
#define EM_CODE_BITS                     16U
#define EM_CODE_MASK                     EM_MASK(EM_CODE_BITS,EM_CODE_SHIFT)

#define EM_FACILITY_SUB_CODE_SHIFT       16U
#define EM_FACILITY_SUB_CODE_BITS        8U
#define EM_FACILITY_SUB_CODE_MASK        EM_MASK(EM_FACILITY_SUB_CODE_BITS,EM_FACILITY_SUB_CODE_SHIFT)
#define EM_FACILITY_CODE_SHIFT           24U
#define EM_FACILITY_CODE_BITS            6U
#define EM_FACILITY_CODE_MASK            EM_MASK(EM_FACILITY_CODE_BITS,EM_FACILITY_CODE_SHIFT)
#define EM_SEVERITY_SHIFT                30U
#define EM_SEVERITY_BITS                 2U
#define EM_SEVERITY_MASK                 EM_MASK(EM_SEVERITY_BITS,EM_SEVERITY_SHIFT)

/* Severity codes */

#define EM_SEVERITY_SUCCESS               0U
#define EM_SEVERITY_INFORMATIONAL         1U
#define EM_SEVERITY_WARNING               2U
#define EM_SEVERITY_ERROR                 3U

 /* List of codes common to all modules */

#define EM_PENDING                        1U
#define EM_INVALID_PARAM                  2U    /* wrong parameter passed               */
#define EM_NOT_IMPLEMENTED                3U    /* for protocol features.               */
#define EM_MEM_ALLOC_FAILED               4U    /* not enough memory                    */
#define EM_UNKNOWN_ERROR                  5U    /* no specific error code               */
#define EM_NOT_SUPPORTED                  6U    /* for invalid operations               */
#define EM_NOT_PRESENT                    7U    /* could not get requested item */
#define EM_BUFFER_TOO_SMALL               8U    /* too little space supplied    */
#define EM_SOFTWARE_EXPIRED               9U    /* s/w evaluation period expired*/
#define EM_INVALID_FSM_STIMULUS           10U    /* stim incompatible with state */
#define EM_INVALID_RESULT_ARG             11U    /* msgPtr in result not set     */
#define EM_LIBRARY_NOT_INTIALIZED         12U    /* InitLibrary not called               */
#define EM_INSUFFICIENT_INFO              13U    /* not enough information supplied*/
#define EM_INVALID_SEQ_NUMBER             14U    /* sequence number mismatch     */
#define EM_FSM_NO_OPERATION               15U    /* state machine stim caused NOOP*/
#define EM_CODE_MAX                       256U

/*
 * Well known facility codes
 */

#define FACILITY_RTP                              3U

#define EM_STATUS(SEV,FAC,SUB,CODE)\
	(((SEV) << EM_SEVERITY_SHIFT) & EM_SEVERITY_MASK) |\
	(((FAC) << EM_FACILITY_CODE_SHIFT) & EM_FACILITY_CODE_MASK)     |\
	(((SUB) << EM_FACILITY_SUB_CODE_SHIFT) & EM_FACILITY_SUB_CODE_MASK) |\
	(((CODE) << EM_CODE_SHIFT) & EM_CODE_MASK)

#define RTP_SUCC(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_WARN(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_INFO(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_ERR(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)



#define EM_RTP_TOO_MANY_CONTEXTS                    (EM_CODE_MAX + 1)
#define EM_RTP_UNKNOWN_CONTEXT                      (EM_CODE_MAX + 2)
#define EM_RTP_NOSOCKET                             (EM_CODE_MAX + 3)
#define EM_RTP_CANT_GET_SOCKET                      (EM_CODE_MAX + 4)
#define EM_RTP_CANT_BIND_SOCKET                     (EM_CODE_MAX + 5)
#define EM_RTP_CANT_SET_SOCKOPT                     (EM_CODE_MAX + 6)
#define EM_RTP_SOCKET_WRITE_FAILURE                 (EM_CODE_MAX + 7)
#define EM_RTP_SOCKET_READ_FAILURE                  (EM_CODE_MAX + 8)
#define EM_RTP_SOCKET_MISMATCH                      (EM_CODE_MAX + 9)
#define EM_RTP_FIXED_WHEN_OPEN                      (EM_CODE_MAX + 10)
#define EM_RTP_BAD_ADDR                             (EM_CODE_MAX + 11)
#define EM_RTP_BAD_PORT                             (EM_CODE_MAX + 12)
#define EM_RTP_CANT_SEND                            (EM_CODE_MAX + 13)
#define EM_RTP_CANT_CLOSE_SESSION                   (EM_CODE_MAX + 14)
#define EM_RTP_BAD_MI                               (EM_CODE_MAX + 15)
#define EM_RTP_BAD_PROFILE                          (EM_CODE_MAX + 16)
#define EM_RTP_SOCKET_EMPTY                         (EM_CODE_MAX + 17)
#define EM_RTP_PACKET_LOOPBACK                      (EM_CODE_MAX + 18)
#define EM_RTP_CSRC_LIST_NOT_SET                    (EM_CODE_MAX + 19)
#define EM_RTP_CSRC_APPEARS_TWICE                   (EM_CODE_MAX + 20)
#define EM_RTP_BAD_PACKET_FORMAT                    (EM_CODE_MAX + 21)
#define EM_RTP_NO_SUCH_PERSON                       (EM_CODE_MAX + 22)
#define EM_RTP_PERSON_DATA_PROTECTED                (EM_CODE_MAX + 23)
#define EM_RTP_NO_SUCH_SENDER                       (EM_CODE_MAX + 24)
#define EM_RTP_NO_SUCH_SDES                         (EM_CODE_MAX + 25)
#define EM_RTP_CANT_USE_ENCRYPTION                  (EM_CODE_MAX + 26)
#define EM_RTP_DECRYPTION_FAILURE                   (EM_CODE_MAX + 27)
#define EM_RTP_END_OF_LIST                          (EM_CODE_MAX + 28)
#define EM_RTP_CANT_CALL_FUNCTION                   (EM_CODE_MAX + 29)
#define EM_RTP_SSRC_IN_USE                          (EM_CODE_MAX + 30)
#define EM_RTP_CANT_GETSOCKNAME                     (EM_CODE_MAX + 31)
#define EM_RTP_NO_SUCH_CONTRIBUTOR                  (EM_CODE_MAX + 32)
#define EM_RTP_CSRC_LIST_FULL                       (EM_CODE_MAX + 33)
#define EM_RTP_DONT_SEND_NOW                        (EM_CODE_MAX + 34)
#define EM_RTP_CANT_SEND_BYE                        (EM_CODE_MAX + 35)



typedef enum {
  RTP_OK =                   0,                                   /* No error */
  RTP_CANT_ALLOC_MEM =       RTP_ERR(EM_MEM_ALLOC_FAILED),         /* Memory allocation failed */
  RTP_TOO_MANY_CONTEXTS =    RTP_ERR(EM_RTP_TOO_MANY_CONTEXTS), /* Max # of contexts exceeded */
  RTP_UNKNOWN_CONTEXT =      RTP_ERR(EM_RTP_UNKNOWN_CONTEXT),         /* Context never created, or destroyed */
  RTP_NOSOCKET =             RTP_ERR(EM_RTP_NOSOCKET),                /* Socket not open for r/w */
  RTP_CANT_GET_SOCKET =      RTP_ERR(EM_RTP_CANT_GET_SOCKET),           /* Can't obtain a new socket */
  RTP_CANT_BIND_SOCKET =     RTP_ERR(EM_RTP_CANT_BIND_SOCKET),          /* Can't bind the socket */
  RTP_CANT_SET_SOCKOPT =     RTP_ERR(EM_RTP_CANT_SET_SOCKOPT),          /* Unable to set certain socket options */
  RTP_SOCKET_WRITE_FAILURE = RTP_ERR(EM_RTP_SOCKET_WRITE_FAILURE),      /* Unable to write to a socket */
  RTP_SOCKET_READ_FAILURE =  RTP_ERR(EM_RTP_SOCKET_READ_FAILURE),       /* Unable to read from a socket */
  RTP_SOCKET_MISMATCH =      RTP_ERR(EM_RTP_SOCKET_MISMATCH),           /* Socket does not belong to session */
  RTP_FIXED_WHEN_OPEN =      RTP_ERR(EM_RTP_FIXED_WHEN_OPEN),           /* change not allowed during open session */
  RTP_BAD_ADDR =             RTP_ERR(EM_RTP_BAD_ADDR),                 /* The address specified is bad */
  RTP_BAD_PORT =             RTP_ERR(EM_RTP_BAD_PORT),                 /* The port specified is bad */
  RTP_CANT_SEND =            RTP_ERR(EM_RTP_CANT_SEND),         /* Sending packets not allowed because
									   destination address was never set */
  RTP_CANT_CLOSE_SESSION =   RTP_ERR(EM_RTP_CANT_CLOSE_SESSION),       /* Unable to close the current session */
  RTP_BAD_MI =               RTP_ERR(EM_RTP_BAD_MI),                   /* No such membership info */
  RTP_BAD_PROFILE =          RTP_ERR(EM_RTP_BAD_PROFILE),              /* Invalid rtpprofile specified */
  RTP_SOCKET_EMPTY =         RTP_WARN(EM_RTP_SOCKET_EMPTY),             /* no data to receive at socket */
  RTP_PACKET_LOOPBACK =      RTP_WARN(EM_RTP_PACKET_LOOPBACK),          /* packet sent from local address with
									   SSRC identical to current SSRC or
									   identical to the previous SSRC
									   (before a collision).  i.e. it
									   is believed the packet looped back */

  RTP_INSUFFICIENT_BUFFER =  RTP_ERR(EM_BUFFER_TOO_SMALL),      /* allocated buffer too small */
  RTP_CSRC_LIST_NOT_SET =    RTP_ERR(EM_RTP_CSRC_LIST_NOT_SET), /* CSRC list not yet set */
  RTP_CSRC_APPEARS_TWICE=    RTP_WARN(EM_RTP_CSRC_APPEARS_TWICE),       /* 2 CSRCs on list have same SSRC */
  RTP_BAD_PACKET_FORMAT =    RTP_WARN(EM_RTP_BAD_PACKET_FORMAT),        /* Packet has incorrect format */
  RTP_NO_SUCH_PERSON =       RTP_ERR(EM_RTP_NO_SUCH_PERSON),           /* Person not session member */
  RTP_PERSON_DATA_PROTECTED =RTP_ERR(EM_RTP_PERSON_DATA_PROTECTED) ,    /* Not allowed to alter this field of data
									   for this person */
  RTP_NO_SUCH_SENDER =       RTP_ERR(EM_RTP_NO_SUCH_SENDER),           /* No such sender in current session */
  RTP_NO_SUCH_SDES =         RTP_ERR(EM_RTP_NO_SUCH_SDES),             /* No such SDES field */
  RTP_CANT_USE_ENCRYPTION =  RTP_ERR(EM_RTP_CANT_USE_ENCRYPTION),       /* Encryption /decryption cannot be used
									   because the encryption / decryption
									   functions were not set. */
  RTP_DECRYPTION_FAILURE =   RTP_ERR(EM_RTP_DECRYPTION_FAILURE),       /* Packet was unsuccessfully decrypted */
  RTP_END_OF_LIST =          RTP_INFO(EM_RTP_END_OF_LIST),              /* Called by iterator when no more members
									     exist on the list */
  RTP_CANT_CALL_FUNCTION =   RTP_WARN(EM_RTP_CANT_CALL_FUNCTION),       /* Function call not allowed at this
									   time (to prevent recursion that would
									   cause side effects) */
  RTP_SSRC_IN_USE =          RTP_WARN(EM_RTP_SSRC_IN_USE),              /* Attempt to create a CSRC with an
									   SSRC value already used in the CSRC
									   list */
  RTP_CANT_GETSOCKNAME =     RTP_ERR(EM_RTP_CANT_GETSOCKNAME),         /* Couldn't get socket name */
  RTP_BAD_VALUE =            RTP_ERR(EM_INVALID_PARAM),                /* Parameter out of range */
  RTP_NO_SUCH_CONTRIBUTOR =  RTP_ERR(EM_RTP_NO_SUCH_CONTRIBUTOR),      /* CSRC List member set to someone
									    not a contributor */
  RTP_CSRC_LIST_FULL =       RTP_ERR(EM_RTP_CSRC_LIST_FULL),           /* The CSRC list has 15 members */
  RTP_DONT_SEND_NOW =        RTP_INFO(EM_RTP_DONT_SEND_NOW),            /* A packet was reconsidered and should be
									     sent later */
  RTP_ERROR_CANT_SEND_BYE =  RTP_INFO(EM_RTP_CANT_SEND_BYE)             /* An attempt was made to send a BYE packet
									   when no RTP or RTCP has been sent previously */
} rtperror;

char *RTPStrError(rtperror err);

#endif
