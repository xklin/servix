
TransportString =
"MP2T/DVBC/UDP;unicast;" ..
"client=801830456511;bandwidth=3750000;" ..
"destination=172.30.81.86;client_port=1234;" ..
"sop_name=SOP1;sop_group=SOPG1"

SetupBodyString =
"v=0\r\n" ..
"o=- 80183045651100000002 0 IN IP4 127.0.0.1\r\n" ..
"s=\r\n" ..
"t=0 0\r\n" ..
"a=X-playlist-item: coship 21M 0-\r\n" ..
"c=IN IP4 0.0.0.0\r\n" ..
"m=video 0 udp MP2T\r\n"



function GetRandOdsid (seed)

    return tostring (seed)
end




function OnAnnounce (session, msg)

    succ, total_message = get_msg (msg)
    print ("Recieved ANNOUNCE message :\n" .. total_message .. "\n")
end


function OnSetupAck (session, msg)

    succ, total_message = get_msg (msg)
    print ("Recieved ack message :\n" .. total_message .. "\n")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
    else
        succ, sid = get_hdr (msg, "Session")
        Play (session, sid)
    end
end


function OnPlayAck (session, msg)

    succ, total_message = get_msg (msg)
    print ("Recieved ack message :\n" .. total_message .. "\n")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
    else
        succ, sid = get_hdr (msg, "Session")
        Teardown (session, sid)
    end
end


function OnTeardownAck (session, msg)

    succ, total_message = get_msg (msg)
    print ("Recieved ack message :\n" .. total_message .. "\n")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
    else
        print ("Now stop the session")
        session_stop (session)
    end
end



function OnRecvFail (session)

    print ("Recieved message failed")
end


function OnRecvUnexpect (session, msg)

    string = get_msg (msg)
    print ("Unexpected message recieved :\n" .. string)
end






function Setup (session)

    succ, msg = new_msg (true)

    fill_reqline (msg, "SETUP", "172.20.100.50", "10086", "RTSP", "1.0")
    fill_hdr (msg, "Require", "com.comcast.ngod.r2")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "StartPoint", "1 0")
    fill_hdr (msg, "SessionGroup", "SSG.SessionGroup1")
    fill_hdr (msg, "Transport", TransportString)
    fill_body (msg, SetupBodyString, "application/sdp")

    odsid = GetRandOdsid (session)
    fill_hdr (msg, "OnDemandSessionId", odsid)


    send_msg (msg, session, "Setupped", "SendFail", 1000)
    reg_on_ack (session, "OnRecvUnexpect")
end


function Setupped (session)

    print ("Sending Setup message success")
    reg_on_ack (session, "OnSetupAck")
end



function Play (session, session_id)

    succ, msg = new_msg (true)

    fill_reqline (msg, "PLAY", "172.20.100.50", "10086", "RTSP", "1.0")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "Require", "com.comcast.ngod.c1")
    fill_hdr (msg, "Range", "npt=0.0-")
    fill_hdr (msg, "Session", session_id)

    send_msg (msg, session, "Played", "SendFail", 1000)
    reg_on_ack (session, "OnRecvUnexpect")
end

function Played (session)

    print ("Sending Play message success")
    reg_on_ack (session, "OnPlayAck")
end




function Teardown (session, session_id)

    succ, msg = new_msg (true)

    fill_reqline (msg, "TEARDOWN", "172.20.100.50", "10086", "RTSP", "1.0")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "Require", "com.comcast.ngod.r2")
    fill_hdr (msg, "Reason", "200 \"user press stop\"")
    fill_hdr (msg, "Session", session_id)
    fill_hdr (msg, "OnDemandSessionId", GetRandOdsid(session))

    send_msg (msg, session, "DTeardown", "SendFail", 1000)
    reg_on_ack (session, "OnRecvUnexpect")
end

function DTeardown (session)

    print ("Sending TEARDOWN message success")
    reg_on_ack (session, "OnTeardownAck")
end


function SendFail (session)

    print ("Sorry, Sending failed")
end






function ConnectSucc (session)

    print ("Connection completed")
    Setup (session)
    reg_on_announce (session, "OnAnnounce")
    reg_on_ack (session, "OnRecvUnexpect")
    reg_on_fail (session, "OnRecvFail")
end


function ConnectFail (session)

    print ("Connection failed")
end



function StartSession ()

    succ, session = session_start ("172.20.100.50", 10086, 1,
                       "ConnectSucc", "ConnectFail", 1000)

    number = number - 1
    if number == 0
    then
        del_timer (timerd)
    end
end


set_syslog_level (6)
number = 9
succ, timerd = set_timer ("StartSession", 200, 200, 0)
succ, session = session_start ("172.20.100.50", 10086, 1,
                       "ConnectSucc", "ConnectFail", 1000)

