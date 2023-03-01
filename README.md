# fmanager
## Установка
Клонируем репозиторий, переходим в папку проекта, затем билдим динамическую библиотеку:

```
cd lib
mkdir build
cd build
cmake ..
make
```

Затем надо вернуться в папку проекта и затем сбилдить весь проект:

```
mkdir build
cd build
cmake ..
make
```

Запустить программу: ```./fmanager```

## Что программа умеет

При старте выводит список файлов/директорий, отсортированный по алфавиту. Перемещаться нужно стрелочками вверх/вниз, удалить файл/папку - ```shift + D```, скопировать файл - ```shift + C```, 
вырезать файл - ```shift + X```, вставить файл - ```shift + V```, не показывать скрытые файлы - ```shift + H```, выйти из программы - ```shift + Q```.
Программа также отображает отдельными цветами симлинки и FIFO. Если нажать ```Enter``` на директорию, то программа перейдет в эту папку, если нажать
```Enter``` на файл, то для некоторых расширений запустится другая программа от этого файла (для расширений ```.txt, .c, .cpp, .h, .S``` откроется вим, для
расширения ```.py``` откроется интерпретатор питона, файлы ```.sh``` будут просто запускаться, файлы ```.gz``` разархивируются).

Программа умеет отображать имя файла, права доступа и размер в байтах (если хватает места на экране терминала). Внизу находится строка состояния, в которой
по умолчанию находится путь до текущей папки, а в случае ошибки там отображается сообщение. При копировании или вырезании файла программа копирует
путь в системный буфер обмена.

В проекте реализованы все пункты, кроме реализации без ncrurses.

UPD: чтобы программа увидела динамическую библиотеку ```.so```, она должна лежать вместе с бинарем ```fmanager```
