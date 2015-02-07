lines = [line.strip() for line in open('html.html')]

s=""
l5 = []

for line in lines:
	s = s + line
	#if (len(s) > 300):
	l5.append(s)
	s=""

l5.append(s)


f = open("html.ino", "w")
f.write('void SendHTML(Stream* client)\n')
f.write('{\n')
f.write('  client->println("HTTP/1.1 200 OK");\n')
f.write('  client->println(F("Content-Type: text/html"));\n')
f.write('  client->println(F("Connection: close"));  // the connection will be closed after completion of the response\n')
f.write('  client->println("");\n')

for l in l5:
	f.write('  client->println(F("' + l.replace('"','\'') +'"));\n')

f.write('client->println("");\n')
f.write('}\n')
f.close()


