# SSHTunnelVpn (SSH + HTTP Proxy)
## For Console Type Application
### Prerequisites

Before starting, make sure you have the following installed on your Linux system:

- **SSH**: To create a secure SSH tunnel.
- **sshpass**: A non-interactive SSH password provider.
- **corkscrew**: A tool to tunnel SSH through HTTP proxies.

You can install these dependencies using the package manager for your system.

#### Install Dependencies:

##### On Debian/Ubuntu-based systems:
```bash
sudo apt update
sudo apt install ssh sshpass corkscrew
```

### Configuration

#### Edit the `settings.ini` file:

1. Locate the `console/c++/settings.ini` file in the repository directory.
2. Open it in a text editor and provide the following details:
   
   - `ssh_host`: The SSH server address (e.g., `www.vpnjantit.com`).
   - `ssh_port`: The SSH port number.
   - `ssh_username`: Your SSH username.
   - `ssh_password`: Your SSH password (if not using key-based authentication).

### Usage
#### 1. Start the SSH Tunnel:
   After configuring the settings.ini file, build and run the tunnel.cpp in `console/c++` folder with g++. You can do this by the following command:
    ```
    g++ tunnel.cpp -o tunnel && ./tunnel
    ```
    
#### 2. Change Browser Proxy Settings:
To use the SSH tunnel for your browser's internet connection:

- Open your browser's proxy settings.
- Set the HTTP Proxy to 127.0.0.1 and the port to 1080.

Save the settings.

### 3. Verify:
Now, your browser will route its traffic through the SSH tunnel. To verify that the proxy is working, you can visit a site like https://www.whatismyip.com to confirm your IP address has changed.

## For QT Desktop Application
### Prerequisites
Before starting with the QT desktop application, ensure the following dependencies are installed on your system:

- QT: The QT framework should be installed. You can install it via your package manager or download it from the official QT website.

### Configuration
After building QT application, you need to provide the ssh credential in the desktop application settings.

### Lets go
Press `Connect` to connect to the server.

Now need to change browser proxy as mentioned before.

### Troubleshooting:
- **SSH Connection Issues:** If you're having trouble with the SSH connection, double-check your settings.ini file for typos or incorrect details. Ensure the SSH server is reachable and the credentials are correct.

- **Firewall Issues:** Ensure that the SSH server allows connections through the specified port and the firewall isn’t blocking the tunnel.

- **Proxy Settings:** Make sure that the proxy settings are correctly applied in the Qt application.

If you want to contibute, create a ticket and create PR.
