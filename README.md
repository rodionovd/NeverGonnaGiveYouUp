# NeverGonnaGiveYouUp

A kernel extension that prevents a userland agent from being killed[^1]. It's built on top of TrustedBSD's MAC policies.

[^1]: By not delivering any destructive signals like SIGKILL or SIGINT

## Usage

#### Building

1. Clone this repository

    ```shell
$ git clone https://github.com/rodionovd/NeverGonnaGiveYouUp
$ cd ./NeverGonnaGiveYouUp
    ```

2. Build the project with Xcode.app or via `xcodebuild`:

    ```shell
$ xcodebuild -project ./NeverGonnaGiveYouUp.xcodeproj -target NeverGonnaGiveYouUp
    ```

3. Copy the build results (the kext and the agent) into a target machine.

#### Installing

1. If you don't own a kext signing cerificate from Apple, you need to disable a few checks on your machine in order to install unsigned kexts

    * For OS X 10.10 (Yosemite)

        ```shell
$ sudo nvram boot-args=kext-dev-mode=1
$ sudo reboot
        ```

    * On OS X 10.11 (El Capitan) there is no «kext-dev-mode» so you must disable System Integrity Protection to play with unsigned kexts. You can learn how to do this [here](http://internals.exposed/blog/dtrace-vs-sip.html). I guess this one should be enough:

        ```shell
$ csrutil enable --without kext
        ```

2. Fix file permissions for the kext: must be `root:wheel`, nonwritable by group/other

    ```shell
$ sudo chown -R root:wheel NeverGonnaGiveYouUp.kext
    ```

3. Load the thing!

    ```shell
$ sudo kextload NeverGonnaGiveYouUp.kext
    ```

    There shall be no errors, otherwise please file a bug!

#### In the wild

Now launch the agent and try to kill it some way. If you open Console.app, you'll see a handy message like:

```
7:15:01 AM kernel: NGGYU: User 501 tried to kill our agent with signal 9. What a pity!
```

------

*Dmitry Rodionov, 2015*  
*i.am.rodionovd@gmail.com*
