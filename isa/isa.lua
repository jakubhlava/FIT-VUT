isa_proto = Proto("ISA", "ISA Protocol")

msg_type = ProtoField.string("isa.type", "Message type")
msg_cmd = ProtoField.string("isa.cmd", "Requested command")
msg_resptype = ProtoField.string("isa.resptype", "Response status")
msg_respcontent = ProtoField.string("isa.respcontent", "Response message")
auth_name = ProtoField.string("isa.auth.name", "Username")
auth_pwd = ProtoField.string("isa.auth.pwd", "Password hash")
auth_token = ProtoField.string("isa.auth.token", "Token")
send_dest = ProtoField.string("isa.send.dest", "Destination")
send_message = ProtoField.string("isa.send.message", "Message body")
fetch_id = ProtoField.string("isa.fetch.id", "Message ID")
fetch_from = ProtoField.string("isa.fetch.from", "From")
fetch_subject = ProtoField.string("isa.fetch.subject", "Subject")
fetch_header = ProtoField.string("isa.fetch.header", "Fetched message")
list_messages = ProtoField.uint32("isa.list.messages", "Listed messages")

isa_proto.fields = { msg_cmd, msg_type, msg_resptype, msg_respcontent, auth_name, auth_pwd, auth_token, send_dest, send_message, fetch_id, fetch_from, fetch_subject, fetch_header, list_messages }

rx_param = GRegex.new("\".+?\"") -- mozna \"(\\\"|.)+?\"
rx_data = GRegex.new("\\(\\d \".+?\" \".+?\"\\)")
rx_num = GRegex.new("\\d")

-- rozhodne o typu zpravy - request/response i obsah, pokud je typ zpravy neznamy, vraci none
function get_message(buffer)
	local message = "none"
	local message_type = "none"
	if buffer(1,2):string() == "ok" then
		message = "OK"
		message_type = "response"
		if buffer(4,1):string() == "(" then
			if buffer(5,1):string() == "(" then
				message_type = "list_response"
			elseif buffer(5,1):string() == "\"" then
				message_type = "fetch_response"
			end
		end
	elseif buffer(1,3):string() == "err" then
		message = "Error"
		message_type = "response"
	elseif buffer(1,8):string() == "register" then
		message = "Register user"
	elseif buffer(1,5):string() == "login" then
		message = "Login user"
	elseif buffer(1,4):string() == "list" then
		message = "List messages"
	elseif buffer(1,4):string() == "send" then
		message = "Send message"
	elseif buffer(1,5):string() == "fetch" then
		message = "Fetch message"
	elseif buffer(1,6):string() == "logout" then
		message = "Logout user"
	end
	if message ~= "none" and message_type == "none" then
		message_type = "request"
	end
	return message, message_type
end

-- extrahuje po sobě jdoucí sekvence parametrů v "" 
function get_params(buffer, count, start)
	if start == nil then start = 0 end
	bufstr = buffer(0, buffer:len()-1):string()
	parsed = {}
	pos = start
	for i = 0, count-1 do
		si, ei = rx_param:find(bufstr, pos)
		if si == nil then return nil, nil end
		parsed[i] = string.sub(bufstr, si+1, ei-1)
		pos = ei+1
	end
	return parsed, pos
end

-- hledá zprávy uložené v ()
function get_data(buffer)
	bufstr = buffer(0, buffer:len()-1):string()
	parsed = {}
	pos = 0
	si = 0
	i = 0
	while si ~= nil do
		si, ei = rx_data:find(bufstr, pos)
		if si ~= nil then
			parsed[i] = string.sub(bufstr, si+1, ei-1)
			i = i + 1
		else
			break
		end
		pos = ei+1
	end
	return parsed, i
end

-- zpracovává obsah jednotlivých trojic z funkce get_data
function parse_data(data)
	si, ei = rx_num:find(data)
	index = string.sub(data, si, ei)
	si, ei = rx_param:find(data)
	arg1 = string.sub(data, si+1, ei-1)
	si, ei = rx_param:find(data, ei+1)
	arg2 = string.sub(data, si+1, ei-1)
	return index, arg1, arg2
end

function isa_proto.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = isa_proto.name

	local subtree = tree:add(isa_proto, buffer(), "ISA Protocol Data")
	cmd, type = get_message(buffer)
	subtree:add(msg_type, type)
	if type == "request" then 
		command = subtree:add(msg_cmd, cmd)
		if cmd == "Register user" or cmd == "Login user" then
			data, i = get_params(buffer, 2)
			command:add(auth_name, data[0])
			command:add(auth_pwd, data[1])
		else
			data, start = get_params(buffer, 1)
			command:add(auth_token, data[0])
			if cmd == "Send message" then
				local msg_data = get_params(buffer, 3, start)
				command:add(send_dest, msg_data[0])
				command:add(fetch_subject, msg_data[1])
				command:add(send_message, msg_data[2])
			elseif cmd == "Fetch message" then
				local message_id = string.sub(buffer(0,length):string(), start+1, length-1)
				command:add(fetch_id, message_id)
			end
		end	
	else
		command = subtree:add(msg_resptype, cmd)
		if type == "response" then
			data, i = get_params(buffer, 1)
			if data ~= nil then 
				subtree:add(msg_respcontent, data[0])
			end
		elseif type == "fetch_response" then
			messages_header = subtree:add(fetch_header)
			params, pos = get_params(buffer, 3, 0)
			messages_header:add(fetch_from, params[0])
			messages_header:add(fetch_subject, params[1])
			messages_header:add(send_message, params[2])
		elseif type == "list_response" then
			messages, count = get_data(buffer)
			messages_sub = subtree:add(list_messages, count)
			
			for i=0, count-1 do
				index, sender, subject = parse_data(messages[i])
				msg_sub = messages_sub:add(fetch_id, index)
				msg_sub:add(fetch_from, sender)
				msg_sub:add(fetch_subject, subject)
			end
		end
	end

end

local tcp_table = DissectorTable.get("tcp.port")
tcp_table:add(32323, isa_proto)
