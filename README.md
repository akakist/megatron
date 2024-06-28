**Megatron**

Сервисно-оринтироеанный фреймворк на C++.

Рассмотрим типичную ситуацию, в которой бывают очень многие достаточно опытные программисты.
Нужно сделать таймер.
Eго делают так - создают новый поток, в него кидают лямбда функцию, в потоке ждут таймаут и выполняют лямбду.
Какие последствия такого действия?
1. Лямбда выполняется в другом потоке.
2. Все переменные, которые совместно используются классом, из которого вызвали таймер, нужно защитить мьютексами.
3. Затем нужно бороться с дедлоками.
В итоге ради реализации тривиального таймера мы создали такой ужас в коде, который исправить практически невозможно.
Причем сначала мы будем делать без мьютексов. Без нагрузки оно будет работать. Под нагрузкой - падать.
Затем мы пытаемся защититься мьютексами от одновременного доступа из двух потоков.
Все равно падает, подвисает на дедлоках. Дальше мы боремся с дедлоками. Все это время состояние проекта - осталось чуть-чуть исправить, а на деле - можно смело выбрасывать.
Стоит ли игра свеч?
Не проще ли сразу применить разумный архитектурный подход, чтобы таймер не создавал новые потоки.
А ведь кроме таймера есть еще много полезных вещей, но без таймера обойтись уж совсем невозможно.

Решение:

Сервис - это модуль, который имеет функцию обработки событий. 
Сервис посылает запрос сервису таймеру типа дай мне алерт через секунду.
Запрос к таймеру ложится в очередь событий, выгребается из нее и обрабатывается сервисом.
Дальше таймер посылает обратное событие в сервис консьюмер и это событие тоже попадает в очередь событий и потом обрабатывается в handleEvent.
Что имеем? Каждый сервис имеет только один мьютекс на очереди событий и блокировки на push и pop. Это сделать куда проще, чем блокировать использование абсолютно всех членов класса. 
handleEvent выполняется в одном потоке, поэтому защищать мьютексом нет необходимости.

Что может Мегатрон?

Скомпиляйте его и подкаталоге build/bin найдите test_http
Он гоняет apach benchmark на вебсервер мегатрона.
Результат: 270 тыс запросов в секунду. Сравниваем с nginx - 195 тыс.
Думаем в чем дело, наш сервер выдает текст из памяти, а nginx - с диска. Делаем чтобы отдавал с диска тот же файл - у нас результат 197,5 тыс запросов.
Работает быстрее чем nginx при прочих равных условиях чуть больше, чем на 1%. И это на C++!
Даже если софт медленнее нгинкса на 20%, то это уже хороший результат, а тут быстрее.

RPC

Мегатрон умеет передавать события по сети на другой нод. Клиент серверная архитектура строится при помощи 2 вызовов - sendEvent(destaddr....), а на сервере - passEvent() - отправить обратным маршрутом.
Просто берем и за 5 минут реализуем. При этом если мы создали облако и событие прошло через несколько нодов, то оно вернется обратным маршрутом. Причем можно послать событие из класса окна GUI, 
если класс отнаследовать от одного класса и вернется ответ также в этот класс GUI.

Можно потратить полчаса на реализацию дисплея для отображения каких-то технологических параметров, которые лежат на сервере.

В build/bin есть тест test_http_rpc
Это хттп сервер, который принимает запрос, затем делает запрос на другой нод, ждет ответ и затем делает ответ на изначально пришедший хттп запрос.
Оцените скорость, у меня показало 117 тыс запросов в сек.
Eще раз - это прием хттп запроса, запрос стороннему серверу, ожидание от него ответа и ответ хттп клиенту. 
Давайте соберем аналогичную систему на Апач кафка, какая там будет скорость? Позже проверю. Даже если Апач кафка не хуже по качеству нгинкса, то там будет одно лишнее прохождение евента до брокера.

То есть мы имеем, что в мегатроне брокер слинкован с handleEvent и не нужно тратить время на вытягивание евентов по сети из брокера.

Другие плюшки:
Сервис телнет. Eму любой сервис может заказать консольные команды, которые прилетят к заказчику, когда юзер введет ее на консоли. Так можно организовать управление бекендом либо просто просматривать какие-то внутренности. 
Примерно как на девайсах cisco.
В телнете автоматом организован help из всех зарегистрированных команд с их описанием и разбиение на каталоги.
Надо написать демку для телнета, возможно, многие захотят им пользоваться.


Система плагинов

Пока мы рассмотрели standalone тесты. В них влинкованы все сервисы, проведена регистрация сервисов ручками, 
даже config file заембедден в код. 
Это чтобы не нужно было настраивать тест каждый раз.

Стандартный режим работы мегатрона - это плагинный. Плагин - это бинарник одного или нескольких сервисов. 
Выполняется в виде динамической библиотеки длл.
Посмотрите код сервиса, например modules/socket
socket.cpp
registerModule - эта функция выполняется при загрузке длл и в качестве параметра 
идет iUtils - это указатель на все глобальные данные мегатрона. 
Сервсис выполняет действия по регистрации самого себя в фабрике сервисов, а также дает понять мегатрону, 
что такие-то сервисы лежат в этом плагине.
Eсли по сети прилетает евент в этот сервис, то система загружает плагин и посылает сервису евент startService, 
на которую делаются различные инициализации  типа установки таймеров и т.п.

Сделайте make install и увидите, что мегатрон установился в каталог ~/bin-10100

В нем есть папки - bin - там лежат exe. 
пробуем запустить  mt-main-10100.exe

Идем в каталог ../conf и видим там mt-main-10100.conf

Снова в bin.
Делаем симлинк mt-main-10100.exe mt-main-10100-web.exe

Запускаем mt-main-10100-web.exe

Смотрим в конфе, появился mt-main-10100-web.conf
То есть на каждый идентичный бинарник мы получили 2 разных конфига.

Смотрим конфиги

обращаем внимание на Start=RPC

Правим строчку: Start=RPC,prodtestServerWeb

Это означает, что мы после запуска мегатрона принудительно стартуем 2 сервиса.
Остальные сервисы стартуют автоматически, получая евенты. Сервисы как бы расталкивают друг друга и стартуют.

Почему нужно руками запустить prodtestServerWeb? Нам нужно чтобы он прибиндился к порту и ожидал соединения, 
извне подтолкнуть его не получится.

Имена сервисов определяются в функции регистрации сервиса.

 iUtils->registerService(COREVERSION,ServiceEnum::prodtestServerWeb,prodtestWebServer::Service::construct,"prodtestServerWeb");

Подробнее о регистрации сервиса.

В ней проводится 2 регистрации.

1. ПлагинИнфо - мегатрон сканирует каталог plugins делает dlopen для них и если есть функция registerModule, то вызывает ее с параметром пути файла. Затем он делает dlclose.
 При прилете сообщения к этому сервису мегатрон знает, что данный сервис находится в этом файле. Он его подгружает и вызывает registerModule где pn - nullptr.
 
2. Идет вызов registerService, сервис регистрируется и стартует. В обработчик евентов прилетает евент startService. Имя сервиса, которое используется в конфиге то, 
что мы указали при регистрации.

После того, как сервис стартовал, ему не хватает каких-то параметров и он дописывает их список с дефолтовыми значениями в конфиг файл.
Так можно просто делать конфиг, меняя созданные параметры на нужные. Адреса нодов можно менять, порты и все что нужно.

Поддержка IPv6 имеется, адреса биндинга или удаленных хостов можно указывать через запятую, можно повесить порт на ipv6 и ipv4.


Почему использован такой формат конфига SocketIO.maxOutBufferSize=8388608 , а не yaml, например. 
Дело в возможности сортировки, то что перед именем параметра стоит имя сервиса - не так существенно.

В функции чтения параметра конфига из кода сервиса нужно указывать строчку описания параметра, 
она автоматически выводится в конфиг перед параметром и после сортировки теряет смысл.

В кодах сервисом можно увидеть такой файл eventgen.sh. Что это такое? Это автоматическая сборка информации об евентах, которые использует сервис. Для чего?
Eсли мы приняли евент из сети, то для обработки евент должен быть зарегистрирован в системе, иначе будет ошибка. Пока все евенты вы не зарегистрируете, 
придется тестить и ловить незарегистрированные. Тот же самый евент может быть зарегистрирован другим сервисом и вы не поймаете ошибку, а где-то на проде поймаете.
Поэтому возникло решение регистрировать вообще все события, которые встречаются в этом коде. 
eventgen.sh сканирует код и составляет хидер, в котором есть функция регистрации событий, которую нужно вызвать около вызова функции регистрации сервиса.
Внутри скрипта отсылка на бинарник, нужно его сбилдить руками.

Делаем свой сервис.

Копипастим какой-то готовый сервис, унаследованный от ListenerBuffered1Thread. Это существенно, если не хотите возиться с многопоточностью внутри сервиса.
Далее меняем имена и делаем свой функционал.
Указали его в конфиге в поле Start и запускаем мегатрон.
Примеры конфигов захардкодлены в тестах, можно там посмотреть еще.

Облако

Мегатрон позволяет создавать иерархическое облако нодов при помощи сервиса dfsReferrer. Eсть тесты для создания облака, там можно посмотреть логику. 
Также там используется сервис dfsCaps, 
но он сейчас упрощен, из него выброшен функционал geoIP. Выбор аплинка осуществляется рандомно.


ЗЫ. Победа над nginx посвящается программистам, которые делали космический челнок Буран. 
Они сотворили на IBM 360 образца 1964г то, что не могут сделать сейчас, имея компьютеры в миллион раз мощнее. Я пытался нащупать и повторить их логику. 
В принципе логика, мегатрона в чем-то повторяет логику языка ДРАКОН, только без графической части, 
но с помощью мегатрона можно реализовать диаграммы активности почти что в визуальном виде.
