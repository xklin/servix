
TransportString =
"MP2T/DVBC/UDP;unicast;" ..
"client=801830456511;bandwidth=3750000;" ..
"destination=80.9.107.1;client_port=1234;" ..
"sop_name=SOP1;sop_group=SOPG1"

SetupBodyString =
"v=0\r\n" ..
"o=- 80183045651100000002 0 IN IP4 127.0.0.1\r\n" ..
"s=\r\n" ..
"t=0 0\r\n" ..
"a=X-playlist-item: coship150 1015 0-\r\n" ..
"c=IN IP4 0.0.0.0\r\n" ..
"m=video 0 udp MP2T\r\n"


function GetC1Addr (body)

    pattern = "a=control:rtsp://(%d+%.%d+%.%d+%.%d+):(%d+)/"
    s, e, ip, port = string.find (body, pattern)
    port = tonumber (port)
    return ip, port
end



function StopSession (session)

    id = session_getid (session)
    r2_conn = SessionTable[id][2]

    if 0 ~= r2_conn
    then
        print ("R2 connection is not 0")
        Teardown (session, r2_conn)
    else
        session_delete (session)
    end
end


function DestroySession (session)

    id = session_getid (session)
    r2_conn = SessionTable[id][2]
    c1_conn = SessionTable[id][3]


    disconnect (r2_conn)

    if 0 ~= c1_conn
    then
        print ("C1 connection is not 0")
        disconnect (c1_conn)
    end

    session_delete (session)
    print ("[" .. id .. "] Session destroyed")
end




function OnAnnounce (connection, msg, session)

    --succ, total_message = get_msg (msg)
    --print ("Recieved ANNOUNCE message" .. total_message .. "\n")
    print ("Recieved ANNOUNCE message")

    succ, msg = new_msg (false) -- Ack message
    fill_statusline (msg, "RTSP", "1.0", "200", "OK")
    fill_hdr (msg, "CSeq", "1")

    sid = session_getsid (session)
    odsid = session_getodsid (session)
    fill_hdr (msg, "Session", sid)
    fill_hdr (msg, "OnDemandSessionId", odsid)

    send_msg (msg, connection, "Announced", "SendFail", 1000, session)
end


function Announced (connection, session)

    print ("Send ANNOUNCE ACK message success\n")
    StopSession (session)
end


function OnSetupAck (connection, msg, session)

    --succ, total_message = get_msg (msg)
    --print ("Recieved ack message " .. total_message .. "\n")
    print ("Recieved SETUP ack message")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
        DestroySession (session)
    else
        reg_on_recv (connection, "OnRecvUnexpect", "OnAnnounce",
                     "OnRecvFail", session)

        succ, body = get_body (msg)
        ip, port = GetC1Addr (body)

        succ, sid = get_hdr (msg, "Session")
        session_setsid (session, sid)
        connect (ip, port, "C1ConnectSucc", "ConnectFail", 1000, session)
        --beatheart (connection, "Get_parameter", 40000, session)
    end
end


function OnPlayAck (connection, msg, session)

    --succ, total_message = get_msg (msg)
    --print ("Recieved play ack message")-- .. total_message .. "\n")
    print ("Recieved PLAY ack message")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
        DestroySession (session)
    else
        reg_on_recv (connection, "OnRecvUnexpect", "OnAnnounce",
                     "OnRecvFail", session)
        --beatheart (connection, "Get_parameter", 40000, session)
    end
end



function OnGetparameterAck (connection, msg, session)

    print ("Recieved GETPARAMETER ack message")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
         StopSession (session)
    end
end


function OnTeardownAck (connection, msg, session)

    --succ, total_message = get_msg (msg)
    --print ("Recieved ack message") -- .. total_message .. "\n")
    print ("Recieved TEARDOWN ack message")

    succ, typed, version, code, desc = get_statusline (msg)

    if code ~= 200
    then
        print ("Unexpected ack code detected : " .. code)
    else
        print ("Now stop the session")
    end

    DestroySession (session)
end



function OnRecvFail (session)

    print ("Recieved message failed")
end


function OnRecvUnexpect (session, msg)

    string = get_msg (msg)
    print ("Unexpected message recieved :\n" .. string)
end






function Setup (session, connection)

    succ, msg = new_msg (true)

    ip, port = get_addr (connection)
    fill_reqline (msg, "SETUP", ip, port, "RTSP", "1.0")
    fill_hdr (msg, "Require", "com.comcast.ngod.r2")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "StartPoint", "1 0")
    fill_hdr (msg, "SessionGroup", "SSG.SessionGroup1")
    fill_hdr (msg, "Transport", TransportString)
    fill_body (msg, SetupBodyString, "application/sdp")

    odsid = session_randodsid (session)
    fill_hdr (msg, "OnDemandSessionId", odsid)

    send_msg (msg, connection, "Setupped", "SendFail", 1000, session)
end


function Setupped (connection, session)

    print ("\nSending Setup message success")
    reg_on_recv (connection, "OnSetupAck", "OnAnnounce",
                 "OnRecvFail", session)
end



function Play (session, connection)

    succ, msg = new_msg (true)

    ip, port = get_addr (connection)
    fill_reqline (msg, "PLAY", ip, port, "RTSP", "1.0")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "Require", "com.comcast.ngod.c1")
    fill_hdr (msg, "Range", "npt=0.0-")

    sid = session_getsid (session)
    fill_hdr (msg, "Session", sid)

    send_msg (msg, connection, "Played", "SendFail", 1000, session)
end

function Played (connection, session)

    print ("Sending Play message success")
    reg_on_recv (connection, "OnPlayAck", "OnAnnounce", "OnRecvFail", session)
end



function Get_parameter (connection, session)

    succ, msg = new_msg (true)
    ip, port = get_addr (connection)

    fill_reqline (msg, "GET_PARAMETER", ip, port, "RTSP", "1.0")
    fill_hdr (msg, "CSeq", "2")
    fill_hdr (msg, "Require", "com.comcast.ngod.r2")
    fill_body (msg, "presentation_state", "text/parameters")

    sid = session_getsid (session)
    fill_hdr (msg, "Session", sid)

    send_msg (msg, connection, "Get_parameterd", "SendFail", 1000, session)
end


function Get_parameterd (connection, session)

    print ("Send GET_PARAMETER message success\n")
    reg_on_recv (connection, "OnGetparameterAck", "OnAnnounce",
                 "OnRecvFail", session)
end




function Teardown (session, connection)

    succ, msg = new_msg (true)

    ip, port = get_addr (connection)
    fill_reqline (msg, "TEARDOWN", ip, port, "RTSP", "1.0")
    fill_hdr (msg, "CSeq", "1")
    fill_hdr (msg, "Require", "com.comcast.ngod.r2")
    fill_hdr (msg, "Reason", "200 \"user press stop\"")

    sid = session_getsid (session)
    fill_hdr (msg, "Session", sid)

    odsid = session_getodsid (session)
    fill_hdr (msg, "OnDemandSessionId", odsid)

    send_msg (msg, connection, "DTeardown", "SendFail", 1000, session)
end

function DTeardown (connection, session)

    print ("Sending TEARDOWN message success")
    reg_on_recv (connection, "OnTeardownAck", "OnRecvUnexpect",
                 "OnRecvFail", session)
end


function SendFail (connection, session)

    print ("Sorry, Sending failed")
    StopSession (session)
end






function R2ConnectSucc (connection, session)

    print ("R2 Connection completed")
    id = session_getid (session)
    SessionTable [id][2] = connection
    Setup (session, connection)
    reg_on_recv (connection, "OnRecvUnexpect", "OnAnnounce",
                 "OnRecvFail", session)
end


function C1ConnectSucc (connection, session)

    print ("Connection completed")
    id = session_getid (session)
    SessionTable [id][3] = connection
    Play (session, connection)
    reg_on_recv (connection, "OnAnnounce", "OnRecvUnexpect",
                 "OnRecvFail", session)
end


function ConnectFail (session)

    print ("Connection failed")
    StopSession (session)
end



function StartSession ()

    succ, session = session_new (1)    -- session group id is 1
    id = session_getid (session)
    SessionTable [id] = {}
    SessionTable [id][1] = session
    SessionTable [id][2] = 0
    SessionTable [id][3] = 0


    connect ("172.20.100.50", 10086, "R2ConnectSucc", "ConnectFail",
             1000, session)

    number = number - 1
    if number == 0
    then
        del_timer (timerd)
    end
end


SessionTable = {}
set_syslog_level (6)
number = 1
succ, timerd = set_timer ("StartSession", 20, 20, 0)

