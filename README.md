### CSC-35 - Lab 2
> Alunos: Bernardo e Victor

Para compilar os arquivos:
```
gcc server.cpp -o server
gcc client.cpp -o client
```

Para executar, abra dois terminais. Em um deles rode:
```
./server
```

No outro, rode:
```
./client localhost
```

O código será executado, realizando a troca de mensagens já programadas no código. Ao final dessa troca, o programa `client` pedirá um input do usuário referente a posição atual do drone. Escreva a posição no formato `número,número,número`.

Para encerrar o protocolo, basta enviar as coordenadas da base: `0,0,0`.
