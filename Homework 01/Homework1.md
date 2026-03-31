# SC4PC2026 - Homework 01

**CloudVeneto Linux VM Setup Guide**

---

## 🎯 Objective

This guide explains how to:

* Register on CloudVeneto
* Create a Linux virtual machine (VM)
* Connect via SSH
* Prepare the environment to compile and run C programs

---

## 1. CloudVeneto Registration

1. Go to: https://cloudveneto.ict.unipd.it/dashboard

2. After clicking on the *Register* button, proceed using one of the following:

   * INFN account (AAI)
   * University of Padua account (SSO)
   * Local account (username & password)

3. Fill in the form with the required information left, and select the project: **SC4PS-PhD**

4. After filling the form, read the AUP that you need to accept. Then click on the Register button and you are done.

5. Wait for approval from the project manager

📩 After approval, you will receive an email with:

* Gate server (e.g. `gate.cloudveneto.it`)
* Username
* Temporary password

### 1.1 SSH host key and password-change troubleshooting

After tunnelling via ssh to the gate (ssh username@gate.cloudveneto.it), if you get an error like this:

```
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!     @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
IT IS POSSIBLE THAT SOMEONE IS DOING SOMETHING NASTY!
...
Offending ECDSA key in ~/.ssh/known_hosts:2
  remove with:
  ssh-keygen -f "~/.ssh/known_hosts" -R "gate.cloudveneto.it"
Host key verification failed.
```

Follow this step exactly as suggested:

```bash
ssh-keygen -f "$HOME/.ssh/known_hosts" -R "gate.cloudveneto.it"
# answer yes to add the new host key
```

If prompted with:

```
(username@gate.cloudveneto.it) You are required to change your password immediately (administrator enforced).
Current password:
(username@gate.cloudveneto.it) New password:
(username@gate.cloudveneto.it) Retype new password:
```

Provide your current password, then your new password twice, and login will succeed.

---

## 2. Create a Virtual Machine

All details are available in the official guide:
https://userguide.cloudveneto.it/en/latest/Overview.html

The procedure described here refers to the specific case of the project associated with the course “Scientific Computing for Physics Students” (SC4PS-PhD).

Any additional required information not included here is either not mandatory or is automatically pre-filled during the registration/completion process.

### 2.1 Access Dashboard

* Log into CloudVeneto
* Go to **Compute → Instances**
* Click **Launch Instance**

---

### 2.2 Instance Configuration

#### 🔹 Details

* **Instance Name:** `your-instance-name`

#### 🔹 Source

* Boot Source: `Image`
* Create New Volume: `No`
* Select image:
  `AlmaLinux10-INFNPadova-YYYY-MM-DD`

#### 🔹 Flavor

* Choose: `cldareapd.large` (or similar)

#### 🔹 Key Pair (IMPORTANT ⚠️)

1. Click **Create Key Pair**
2. Choose:

   * Type: `SSH Key`
   * Name: e.g. `sc4pc-key`
3. Copy the private key immediately
4. Save it locally:

```bash
mkdir -p ~/.ssh
nano ~/.ssh/sc4pc-key
```

Paste the key and save.

Set permissions:

```bash
chmod 600 ~/.ssh/sc4pc-key
```

After that, remember also to set the admin password (flagging this option), before launching the instance.

---

### 2.3 Launch Instance

After launching, note:

* **Instance Name**
* **IP Address**
* **Key Pair name**
* **Status: ACTIVE (if not, activate it in the Actions tab)**

---

## 3. SSH Configuration

### 3.1 Create/Edit SSH Config File

```bash
nano ~/.ssh/config
```

---

### 3.2 Add Configuration

```bash
Host cloudveneto-gate
    HostName gate.cloudveneto.it
    User YOUR_USERNAME
    ForwardX11 yes
    ForwardX11Trusted yes
    ForwardAgent yes

Host myvm
    HostName YOUR_VM_IP
    User almalinux
    IdentityFile ~/.ssh/sc4pc-key
    ProxyCommand ssh -q -W %h:%p gate_cloudveneto
    PubkeyAuthentication yes
    ForwardX11 yes
    ForwardX11Trusted yes
    ForwardAgent yes
```

Replace:

* `YOUR_USERNAME`
* `YOUR_VM_IP`
* `myvm` (alias name)

---

### 3.3 Set Permissions

```bash
chmod 600 ~/.ssh/config
```

---

### 3.4 Connect to VM

```bash
ssh myvm
```

---

## 4. Prepare Environment for C Programming

Once connected to the VM:

### 4.1 Update System

```bash
sudo dnf update -y
```

---

### 4.2 Install GCC Compiler

```bash
sudo dnf install gcc -y
```

Check installation:

```bash
gcc --version
```

---

### 4.3 Install Useful Tools

```bash
sudo dnf install nano vim make -y
```

---

## 5. Compile and Run a C Program

### 5.1 Create a Test File

```bash
nano hello.c
```

Example code:

```c
#include <stdio.h>

int main() {
    printf("Hello, CloudVeneto!\n");
    return 0;
}
```

---

### 5.2 Compile

```bash
gcc hello.c -o hello
```

---

### 5.3 Run

```bash
./hello
```

---

## 6. Notes and Tips

* Default VM user is usually:

  * `almalinux` (for AlmaLinux images)

* If SSH fails:

  * Check key permissions (`chmod 600`)
  * Verify IP address
  * Ensure instance is ACTIVE

* To install software:

```bash
sudo dnf install <package-name>
```

---

## ✅ Summary

You have now:

* Created a CloudVeneto VM
* Connected via SSH
* Installed GCC
* Compiled and executed a C program

---

## 📌 Optional Improvements

* Use `Makefile` for larger projects
* Install debugging tools (`gdb`)
* Configure Git for version control

```bash
sudo dnf install gdb git -y
```

---

**End of Guide**
