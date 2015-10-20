/***************************************************************
   -
   - file:  svtSEEEDCAN.cpp
   -
   - purpose: handles connection to solar car main CAN bus
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#include "svtSEEEDCAN.h" 

/** svtSEEEDCAN - destructor
 */
svtSEEEDCAN::~svtSEEEDCAN(){
}

/** isr - handles all interrupts from MCP2515, sets/clears thread signals 
 *         note signature is (), not (void).
 */
void svtSEEEDCAN::isr(){
    osSignalSet(_threadId, SIG_CAN_MSG_READY);  
}

/** init - set up function,  initializes MCP2515 with required speed, format and sets MPC2515 interrupts
 */
int svtSEEEDCAN::init(int canspeed, CANFormat format, uint32_t canaddr){
    _can.open(canspeed); // defaults to 1Mbs/normal 
    _can.setinterrupts(SEEED_CAN::RxAny);  // tells MCP2515 to use interrupts
    if(_can.errors()){
	SEEED_CANMessage msg;
	uint8_t err = _can.errorFlags();
	udebug.printf("err %d\r\n", err);
	_can.read(msg);
    }
    svtCAN::init(canspeed, format, canaddr);
    return 0;
}

/** Filter - sets global mask and up to six input id filters, which when combined with
 *   mask, determine which messages are read from bus.
 *  @param mask  32 bit mask, where a 1 indicates bit will be checked between
 *     incoming-id and filter-id, 0 is pass all. The underlying hardware may support
 *     more complex masking, but here only single global mask used
 *   @param idfilters - pointer to an array of filters
 *   @param n - number of filters supplied
 *   @return 0 - success, 1 failure
 */
int svtSEEEDCAN::Filter(int32_t mask, int32_t* idfilters, int32_t n){
    // MCP2515 has two masks, both are set to be same here.
    if(_can.mask(0, mask, _format)){
	return 1;
    }
    if(_can.mask(1, mask, _format)){
	return 1;
    }
    if(idfilters == NULL) return 1;
    if( n>6) return 1;
    for(int i=0; i<n; i++){
	if(_can.filter(i, idfilters[i], _format)){
	    return 1;
        }
    }
    return 0;
}

/** Set the frequency of the CAN interface
 *
 *  @param hz The bus frequency in hertz
 *
 *  @returns
 *    1 if successful,
 *    0 otherwise
 */
int svtSEEEDCAN::frequency(int hz){
    return _can.frequency(hz);
}

/** handleInMsg - retrieves msg  and puts in local IN queue
 *                 the read statement clears interrupt on MPC2515
 */
int svtSEEEDCAN::handleInMsg(){
    SEEED_CANMessage canMsg;
    INqueue_mutex.lock();
    {
	if(_can.read(canMsg)){
	    CAN_Message *message = INqueue.alloc();
	    if(message != NULL){
		message->len    = canMsg.len;
		message->type   = canMsg.type;
		message->format = canMsg.format;
		message->id     = canMsg.id;
		memcpy(message->data, canMsg.data, canMsg.len);
		INqueue.put(message);
		osSignalSet(_threadId, SIG_CAN_INQUEUE_NOTEMPTY);
	    }
	}
	else{
	    udebug.printf("X");
	}
    }
    INqueue_mutex.unlock();
    return 0;
}

/** handleOutMsg - retrieve CAN msg from OUTqueue and send SEEED_CANMessag.
 * result codes from mail.get    
 *   osOK: no mail is available in the queue and no timeout was specified
 *   osEventTimeout: no mail has arrived during the given timeout period.
 *   osEventMail: mail received, value.p contains the pointer to mail content.
 *   osErrorParameter: a parameter is invalid or outside of a permitted range.
 */
int svtSEEEDCAN::handleOutMsg(){
    int err;
    if(_canmsg.len != 0){ // old unsent message, try again
	err = _can.write(_canmsg); // err = 1-sent, 0-not sent
	if(err){ // msg sent
	    _canmsg.len = 0;
	}
    }
    else{   // read next message from queue
	OUTqueue_mutex.lock();  // critical section?
	{
	    osEvent evt = OUTqueue.get(0); // no waiting
	    if(evt.status == osEventMail){
		CAN_Message *msg = (CAN_Message*)evt.value.p;
		_canmsg.id      = msg->id;
		_canmsg.type    = msg->type;
		_canmsg.len     = msg->len;
		_canmsg.format  = msg->format;
		memcpy(_canmsg.data, msg->data, msg->len);
		OUTqueue.free(msg); // queue read, so free mem
	    }
	    else if(evt.status == osOK) { // queue empty - clear flag
		osSignalClear(_threadId, SIG_CAN_INQUEUE_NOTEMPTY);
	    }
	}
	OUTqueue_mutex.unlock();  // end critical section
    }
    if(_canmsg.len != 0 ){ // have msg, try to send
	err = _can.write(_canmsg);

	if(err){ // msg sent
	    _canmsg.len = 0;
	}
    }
    return 0;
}






