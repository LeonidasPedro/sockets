import os, socket, sys
host=''
porta=8080
print host+':'+str(porta)
origem = (host, porta)
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(origem)
serversocket.listen(1)
while True:
	con, cliente = serversocket.accept()
	pid = os.fork()
	if pid==0:
		serversocket.close()
		while True:
			mensagem = con.recv(1024)
			print 'Informe o comando a ser enviado'
			mensagem = raw_input()
			if not mensagem: break
			con.send(mensagem)
			os.system(mensagem[:-2])
			con.send('mensagem para o cliente')
		print 'Finalizando cliente'
		serversocket.close()
		sys.exit(0)
	else:
		con.close()