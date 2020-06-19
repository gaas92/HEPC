# HEPC
## Repositorio para el curso de HEP cinvestav2020
Saludos, este repositorio es para ustedes, aqui encontraran los ejercicios que preparamos y las instrucciones para que puedan hacerlos. 
Cualquier comentario o problema tecnico cominiquense conmigo o con Daniel.
## 1 Prerrequisitos:
- Hacer cuenta en github
- Tener una terminal bash y/o accesso ssh 
## 2:
- Si ya tienen su cuenta en github, hay dos formas de continuar. Pueden realizar los ejercicios en su computadora localmente o utilizar el enviroment que hemos dejado preparado para ustedes:
- Para ultilizar el servidor deben conectarse via ssh, pero probablemente el IP de su casa no esta dado de alta para ingresar, asi que si quieren usar esta opcion deben tunelearse al cinvestav,
hay varias formas para lograr esto, aqui les propongo la siguiente solucion:
  - Descargar el programa sshuttle, el repositorio pueden encontrarlo aqui [sshuttle](https://github.com/sshuttle/sshuttle), o deescargarlo e instalarlo directamente via pip:
```
$ sudo pip install sshuttle
```
  - y despues tunelearse a pine con:
```
$ sshuttle --dns -r usuario_de_cinvestav@pine.fis.cinvestav.mx 0/0
```
  - Lo que esto realiza es mandar todo su trafico a pine.(En el caso de Jhovanny, si tu cuenta en pine no esta activa puedes aplicar la misma via higgs)

- Ahora deberian poder conectarse al servidor con:
```
$ ssh username@hcrotte.fis.cinvestav.mx

```
e ingresar su password. Pueden utilizar los argumentos 

Att.
Gabriel.

