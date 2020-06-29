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
- Esto lo que hace es mandar todo su trafico a pine.(En el caso de Jhovanny, si tu cuenta en pine no esta activa puedes aplicar la misma via higgs), les pedira su contraseña local y sus credenciales de cinvestav (mismo password de su correo de fisica), esto deberia permiriles conectarse en multiples seciones ssh, bajar articulos o usar las licencias de software del cinvestav sin mayor problema. Otra opcion mas sencilla es entrar directamente a pine y de ahi al servidor.
  
- Ahora deberian poder conectarse al servidor usando otra terminal con:
```
$ ssh username@hcrotte.fis.cinvestav.mx
```
- Si tienen problemas con sshuttle o simplemente no quieren instalar cosas raras en su computadora, pueden 
e ingresar su password, (se las mando a su correo). Si desean cambiarla pueden usar ```$ passwd ```.

- Ahora vamos a intentar clonar este repositorio, lo cual puede realizarse facilmente usando 
```
$ git clone https://github.com/gaas92/HEPC.git
```

- Cuando tengan el repositorio, si trabajan localmente deberian poder correr en su instalacion de ROOT los  macros .C, que estan en la carpeta "tests" usando  ```$ root -l macro_.C``` y el python usando  ``` $ python rf101_basics.py ``` y deberia correr correctamete, si tienen problemas pueden contactarnos o usar el servidor. 

- Si trabajan en el servidor primero deben setear el virtual enviroment que Horacio nos hizo favor de configurar:

```
$ cd HEPC
$ conda activate root_env
```
- Asi deben ser capacer de correr cualquier ejercicio de la carpeta "tests".

- Espero sus sugerencias :+1: :shipit:

## Seccion de Ivan
Esta parte utiliza la interfaz grafica de root, si queremos usar  a hcrotte.fis.cinvestav.mx, debemos conectarnos con la opcion -X (untrusted X11 forwarding):
```
$ ssh -X username@hcrotte.fis.cinvestav.mx
```
Debido a que esto es tan lento como lo sea el internet del cinvestav (osea mucho), es recomendable correr estos ejercicios localmente (basta tener una instalacion de root)
Salvo instrucciones proximas de Ivan. Lo que debemos hacer para correr estos ejemplos es:

- Dessde el directorio de este repositorio ir a la carpeta Ivan usando :  ```$ cd Ivan/``` . 
- Primero correemos blind.C, ``` $ root -l blind.C ```, esta macreo abre un TCanvas sobre el cual se desplegaran graficas, para avanzar en el ejericio deben hacer click o doble-click sobre las imagenes desplegadas, de nuevo si corren esto en hcrotte.fis.cinvestav.mx, el proceso sera mucho mas lento que hacerlo localmente. 
-Ahora si todo ha funcionando adecuadamente habremos generado un workspace, y podemos correr signif_bkgsub.C: ``` $ root -l signif_bkgsub.C ```. Deberan hacer click o doble-click en el TCanvas para avanzar en el ejercicio. 


Att.
Gabriel.

