# ソフトウェアについて

Windows 7以降で実装されている資格情報プロバイダーのメカニズムを利用して、NFC™に準拠したICカードによるログオン処理を実装しています。

Windows 10だとたぶん無理かも。

# 導入と削除について

`regsvr32.exe`でレジストリーに登録したり、解除できるようになっています。資格情報プロバイダーの追加と削除に関して再起動の必要はありません。

## 導入

次のコマンドラインを管理者権限下で実行すると導入されます。

`regsvr32 NFCCP.dll`

## 削除

次のコマンドラインを管理者権限下で実行すると削除されます。

`regsvr32 /u NFCCP.dll`

# 実装について

`ICredentialProvider`インターフェースを基点として、関連するインターフェースを実装することで独自のログオン処理を実装することができます。

また、この資格情報プロバイダーに対応するLSA認証パッケージを実装しています。

これらの2つを組み合わせることでNFC™に準拠したICカードを使用したアカウンティングが可能になります。

## `ICredentialProvider`インターフェース

資格情報プロパイダーの実体となるインターフェースです。ユーザーの認証処理に関するイベントを受信出来るようにしたり、ログオン画面の入力項目を追加したりする役割を持っています。

参考:[ICredentialProvider interface](https://msdn.microsoft.com/en-us/library/windows/desktop/bb776042.aspx)

## LSA (Local Security Authority)

LSA認証パッケージは外部から入力された資格情報を検証し、認可された場合はトークンを生成して返すコンポーネントです。

参考:[LSA Authentication](https://msdn.microsoft.com/en-us/library/windows/desktop/aa378326.aspx)
