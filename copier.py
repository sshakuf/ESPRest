lines = [line.strip() for line in open('html.html')]

s=""
l5 = []

for line in lines:
	s = s + line
	if (len(s) > 300):
		l5.append(s)
		s=""

l5.append(s)


f = open("html.ino", "w")
f.write('void SendResponse(int ch_id, String Content);\n')

f.write('void SendHTML(int ch_id)\n')
f.write('{\n')
f.write('	String str="";\n')
for l in l5:
	f.write('  str+=F("' + l.replace('"','\'') +'");\n')

f.write('str+=F("");\n')
f.write('SendResponse(ch_id, &str);\n')
f.write('}\n')
f.close()


