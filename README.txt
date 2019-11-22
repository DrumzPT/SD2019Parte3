Não foram implementados com sucesso todos os comandos.
O comando 'put' e o Comando 'getKeys' não estão funcionais
Como tal para poder testar os outros comandos a tabela é inicializada coms os
seguintes valores:
  <key>   | <data>
  123abc  | 1234567abc
  teste   | 1234567abc
  outra   | q1w23er45t
  lmnop   | q1w23er45t

O servidor dá segfault no caso de se tentar obter uma chave que não exista

Phase2 Network.
1. Implement coding/decoding. // protoc --c_out=. sdmessage.proto // Gera ficheiros .c e copiar .h para include

2. Server
	2.1 network_server.
		2.1.1 init.
		2.1.2. main_loop.
		2.1.3 network_receive
		.4 network_send.
		.5 network_close.
	2.2 table_skel.
		.1 init.
		.2 destroy.
		.3 invoke.
	2.3 table_Server.
3 Client
	3.1 table_client.
		.1 size,
		.2 put,
		.3 get.
		.4 del.
		.5 getkeys
		.6 quit.
	3.2 cleint_stub
		.1 connect.
		.2 disconnect.
		.3 size,
		.4 put,
		.5 get.
		.6 del.
		.7 getkeys
		.8 free_keys
	3.3 network_client
		.1 connect.
		.2 send_receive.
		-.3 close.

Garantir que as funcoesdque fazem o malloc() tambem fazem o free().
Implementar signal() mas para isso estudar SIGPIPE, SIGIGN, etc.

Phase 3: Multiplexing client requests
Implement multiplexing with poll() (Server).
Figure out how to launch several clients.
Test server multiplexing with several sequencial requests from different clients.
Adaptar o servidor para processar pedidos de escrita assincronamente e responder sincronamente com o identificador da operacao.
	Implementar fila FIFO que regista pedidos.
		metodos
	Gerar thread process_task() que processa os pedidos na fila
		Retirar pedido da fila
		Executar pedido
		Incrementar op_count
		Sincronizar acesso atomico a variaveis partilhadas
	Alterar invoke() do table_skel.c para inserir pedido de escrita na fila
	Alterar invoke() do table_skel.c para responder com id do pedido last_assigned (para o cliente poder usar no verify())
Implement verify() (Server)
Implement rtable_verify() (Client)