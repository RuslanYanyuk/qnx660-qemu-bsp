#ifndef _PCI_EVENT_H_
#define _PCI_EVENT_H_
/*
 * $QNXLicenseC:
 * Copyright (c) 2012, 2016 QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#include <pci/pci.h>
#include <pthread.h>


/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 PCI server event API's

 The PCI library provides a set of event notification API's that allow the
 caller to easily register a handler function to be called whenever the event
 of interest occurs. This mechanism eliminates the need for the caller to have
 to manage connections or otherwise worry about the details of event delivery
 and instead concentrate on the details of handling the event occurrence.

 When one of the event registration API's (pci_event_reg_*() is called for the
 first time, an event handler thread is created on behalf of the process. The
 'pthread_t' for this thread can be obtained with the pci_event_get_thread_id()
 API.
 After the caller has unregistered for reception of all events, the event
 handler thread terminates.

 The use of the event notification API's requires at least one event processing
 server module to be loaded. See /etc/system/config/pci/pci_server-template.cfg
 for a description on how to load server modules.

 Below is the current list of events which are supported and the server module
 which provides the notification. Note that some modules will handle more than
 one event and there may be more than one module which handles a given event

 1. device insertion and removal      - pci_server-event_handler.so
 2. link state change                 - pci_server-event_handler.so


 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
 ===============================================================================
 pci_event_link_e

 This type is used to identify the type of link event which occurred

*/
typedef enum
{
	pci_event_link_state_UP = (1u << 0),
	pci_event_link_state_DOWN = (pci_event_link_state_UP << 1),

} pci_event_link_e;

/*
 ===============================================================================
 pci_evthdl_t

 this type represents a unique token received from the PCI server when a
 successful event registration call is made. It can be passed to other event
 API's including event de-registration
*/
typedef void *		pci_evthdl_t;

/*
 ===============================================================================
 pci_event_get_thread_id

 When a caller registers for an event, an event handling thread is created to
 handle the event notification and execute the registered callback function.
 To allow software to have complete control over the scheduling attributes of
 this thread, this function will return the 'pthread_t' for the thread created
 to handle the event identified by 'pci_evthdl_t' returned from a successful
 event registration.

 On any error, -1 will be returned

 Note that it is also possible for the registered callback to use pthread_self()
 to obtain the 'pthread_t' for the event handling thread however this function
 allows any thread to easily obtain this information and also prior to event
 occurrence.

*/
pthread_t pci_event_get_thread_id(const pci_evthdl_t evthdl);

/*
 ===============================================================================
 Device insertion

 pci_event_reg_device_insert() and pci_event_unreg_device_insert() will allow
 the caller to register/deregister for notification of a live device insertion
 event for the chassis and slot identified by <cs>. When the insertion occurs,
 the registered handler function (of type 'pci_event_device_insert_f') will be
 called with the BDF of the inserted device.

 Note that the PCI_FUNC() of the 'bdf' argument passed to the registered handler
 will always be zero. The hander can use existing API's to determine whether or
 not the device is a multi-function device and access the other functions
 accordingly

 The <priority> parameter allows the caller to specify the priority of the
 event notification pulse. A value of -1 will cause the pulse to be delivered
 at the priority of the calling process.

 ===============================================================================
*/
typedef void (*pci_event_device_insert_f)(const pci_bdf_t bdf);
pci_evthdl_t pci_event_reg_device_insert(pci_cs_t cs, int_t priority, pci_event_device_insert_f handler, pci_err_t *err);
pci_err_t pci_event_unreg_device_insert(pci_evthdl_t evthdl);

/*
 ===============================================================================
 Device removal

 pci_event_reg_device_remove() and pci_event_unreg_device_remove() will allow
 the caller to register/deregister for notification of a live device removal
 event for the chassis and slot identified by <cs>. When the removal occurs,
 the registered handler function (of type 'pci_event_device_remove_f') will be
 called with the BDF of the removed device.

 Note that the BDF passed to the registered handler may be PCI_BDF_NONE if it
 could not be determined otherwise the PCI_FUNC() of the 'bdf' argument passed
 to the handler will be zero.
 
 The <priority> parameter allows the caller to specify the priority of the
 event notification pulse. A value of -1 will cause the pulse to be delivered
 at the priority of the calling process.

 ===============================================================================
*/
typedef void (*pci_event_device_remove_f)(const pci_bdf_t bdf);
pci_evthdl_t pci_event_reg_device_remove(pci_cs_t cs, int_t priority, pci_event_device_remove_f handler, pci_err_t *err);
pci_err_t pci_event_unreg_device_remove(pci_evthdl_t evthdl);


/*
 ===============================================================================
 PCIe port Link Activity

 pci_event_reg_link_state() and pci_event_unreg_link_state() will allow the
 caller to register/deregister for notification of a PCIe link state event
 for the PCIe link associated with the bridge identified by <bdf>. When a link
 state change occurs (either link up or link down), the registered handler
 function (of type 'pci_event_link_state_f') will be called with the state of
 the link, pci_event_link_state_UP or pci_event_link_state_DOWN. All other link
 event type values should be interpreted as link state unknown.

 The <priority> parameter allows the caller to specify the priority of the
 event notification pulse. A value of -1 will cause the pulse to be delivered
 at the priority of the calling process.

 ===============================================================================
*/
typedef void (*pci_event_link_state_f)(const pci_event_link_e link_state);
pci_evthdl_t pci_event_reg_link_state(pci_bdf_t bdf, int_t priority, pci_event_link_state_f handler, pci_err_t *err);
pci_err_t pci_event_unreg_link_state(pci_evthdl_t evthdl);



#endif	/* _PCI_EVENT_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/lib/pci/public/pci/pci_event.h $ $Rev: 810516 $")
#endif
