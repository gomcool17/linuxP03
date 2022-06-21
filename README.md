# linuxP03

# 1. 개요
본 과제는 리눅스 시스템 상에서 사용자가 동기화를 원하는 파일이나 디렉토리를 동기화 하는 프로그램인 ssu_rsync와 리눅스 시스템 상에서 사용자가 주기적으로 실행하고자 하는 명령어를 등록하고 실행시켜주는 ssu_crontab을 구현한다.
 cron이란 특정한 시간에 또는 특정 시간 마다 어떤 작업을 자동으로 수행하게 해주고 싶을 때 사용하는 명령어이다. cron 시스템에는 시스템에서 기본적으로 사용하는 cron설정이 있으며, 이를 시스템크론이라고 한다 또한 root나 일반 사용자가 자신의 cron설정을 직접하여 사용하는 사용자크론이 있다.
 crontab은 cron작업을 설정하는 파일이다. cron프로세스는 /ect/crontab 파일에 설정된 것을 읽어서 작업을 수행한다. crontab파일은 OS별로 각각 다른 위치에 저장이 된다. cron은 전원이 켜지면 init데몬들 통해 crond데몬을 실행해 파일을 읽어드려 매 시간, 일, 월등 주기적으로 실행해야하는 내용들을 수행한다. 본 과제는 이러한 crontab의 특성을 이해해 직접 명령어를 추가해 저장하는 파일을 만들어 주기별로 수행해주는 디몬 프로세스를 만든다.

---

# 2. 설계
<img width="462" alt="image" src="https://user-images.githubusercontent.com/30142575/174766188-4a7b07d1-9c6c-49fc-8327-a5a84a7b8e28.png">
ssu_crond.c – 이 프로그램을 실행시 ssu_conrtab_file을 읽어드려 파일의 내용을 한줄씩 검사해주어 해당 주기를 확인한다. 만일 프로그램 실행시간이 저장되어있는 주기와 같을시 명령어를 실행시켜준다.


ssu_crontab.c – add나 remove 명령어가 들어올시 해당 명령어를 수행해준다. ssu_crontab_file을 읽으면서 몇개의 명령어가 들어있는지 검사해주어 정보를 통해 번호도 함께 출력해준다. remove가 들어올시 해당 번호를 찾아 저장되어 있는 배열을 한칸씩 당겨서 파일에 다시 overwrite를 해준다  
ssu_rsync.c – 이 프로그램은 2개 혹은 3개의 인자를 전달 받아야 한다. 2개의 인자가 들어올시 파일인 src혹은 디렉토리인 src와 dst디렉토리를 비교확인하여 동기화가 필요할 시 동기화를 수행해준다. 인자는 절대경로일수 있고, 상대경로일 수도 있다.

---

# 3. 테스트 및 결과
+	ssu_rsync
 <img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767295-1665a1a5-df07-40a0-8f73-7625153da06d.png">
<img width="432" alt="image" src="https://user-images.githubusercontent.com/30142575/174767312-e898eddc-169d-4a7f-b8c7-9535857d15d4.png">

  + 수정 후 log확인
  <img width="442" alt="image" src="https://user-images.githubusercontent.com/30142575/174767337-dbd3a884-dc71-4390-9df0-3a4acbdcc767.png">
  + src 파일 인자로 주었을때

<img width="424" alt="image" src="https://user-images.githubusercontent.com/30142575/174767375-54d369df-f2b7-45c3-8ef3-d4ea0e5ed5f9.png">

  + 내용이 제대로 동기화 되었는지
 <img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767407-617ad460-170f-4f01-a7d3-7650e1e81ca0.png">

  + 절대경로 확인
 <img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767424-78b31033-0785-445f-bcdb-958ae1573df2.png">
<img width="350" alt="image" src="https://user-images.githubusercontent.com/30142575/174767435-8a644c58-8fba-4db5-ba85-ca51b0977e65.png">

+	ssu_crontab
  + 입력내용

<img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767482-ed23beff-2f68-4331-b6db-9716cf93fce1.png">
<img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767497-e2593902-7f64-4f16-a659-75234a7f3c86.png">

  + log확인

<img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767535-04c2acbc-8949-48ad-be5b-96cf887304ab.png">
  + 에러처리

<img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767547-3badd604-7b6a-4326-ab56-b1d64786d67f.png">

+ ssu_crond
  + 실행시 ssu_crontab_file에 저장되어있던 명령어들

<img width="441" alt="image" src="https://user-images.githubusercontent.com/30142575/174767596-656a4576-feec-4873-b02c-34139346e162.png">

  + 디몬 프로세스 확인

<img width="452" alt="image" src="https://user-images.githubusercontent.com/30142575/174767623-bbbfc8a1-86c1-407d-a95f-72820f924f0a.png">

  + ssu_crond를 통해 명령어가 실행됨을 확인

<img width="187" alt="image" src="https://user-images.githubusercontent.com/30142575/174767660-72e4840f-b0b0-43e8-9c2f-a1f2ee4842bb.png">
<img width="404" alt="image" src="https://user-images.githubusercontent.com/30142575/174767679-ea4c82c3-61fb-4ccf-9631-5502b73ce646.png">

