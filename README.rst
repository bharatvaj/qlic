qlic
====

qlic is an unecessary fast cli frontend to the venereal Zoho cliq. It is cliq but unopinionated.

NOTE: Built with bubble gum and paper clip. Highly unstable, please use at your own risk.

![cliq-jpeg](images/qlic.jpg)

Why?
----
I avoid browsers like the plague. Zoho cliq was my only reason for using a browser. Not anymore.

What works
----------
* Authentication via OAuth
* Receive and send text to chat, channels
* Receive and send attachments to chat, channels

What doesn't work
-----------------
* Cliq call - Apparently this takes more work than I thought.
* Screenshare - Sharing screen is top priority for qlic, but it requires call to work first.
* Widgets - I have no interest in implementing widgets in a cli as it doesn't benefit me, but contributions are welcome as always.

Unique features
---------------
* stdin support!
	- You can send attachment to qlic's `stdin`
```sh
# NOTE: `nwoo` is the recipient
# qlic tries to match with the closest name, if it matches with multiple names, a prompt will be shown to the user

# Sending a text
echo "hello fren" | qlic -r nwoo
qlic -r nwooo "henlo fren"

# Sending an attachment
cat how-to-basic-101.jpg | qlic -r nwoo -b meme.jpg
```

Install
-------
```sh
git clone https://github.com/bharatvaj/qlic
cd qlic
make install
```

Dependencies
------------
* libcurl
* bharatvaj/OAuth2
* yarosla/nxjson

Setup
-----
* Create a config file at `$XDG_DATA_HOME/qlic/data.json` with the following configuration
```json
{
	client_id: 'your_client_id',
	client_secret: 'your_client_secret',
}
```
NOTE: client id and secret can be obtained from [Zoho's API console](https://api-console.zoho.com/)
