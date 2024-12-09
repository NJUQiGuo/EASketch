Since logging in with the new user account (`testuser`) results in a normal display, this indicates that the issue is related to the configurations or settings specific to your original user account. Here are the steps you can take to resolve this issue:

### Step 1: Backup Current Configuration

Before making any changes, it's important to back up your current configuration in case you need to revert:

```bash
mv ~/.config ~/.config_backup
```

### Step 2: Gradually Restore Configurations

You can restore your configurations gradually to identify which one is causing the issue. Start with critical applications and settings:

```bash
# Create a new .config directory
mkdir ~/.config

# Copy critical application configurations one by one
cp -r ~/.config_backup/application_name ~/.config/
```

After copying each application's configuration, log out and log back in to check if the issue reappears. If the display becomes abnormal after copying a specific configuration, that configuration is likely causing the issue.

### Step 3: Compare Configurations

If you want to compare configurations more systematically, you can use tools like `diff` to compare the configurations of the new user (`testuser`) with your original user:

```bash
diff -r /home/testuser/.config /home/original_user/.config
```

This will help you identify differences that might be causing the display issue.

### Step 4: Resetting to Default Configuration

If you don't need specific configurations and prefer to reset everything to default, you can delete the `.config_backup` and `.config` directories, and start fresh:

```bash
rm -rf ~/.config
rm -rf ~/.config_backup

# Create a new .config directory
mkdir ~/.config
```

After doing this, log out and log back in to see if the issue is resolved.

### Step 5: Check for Hidden Settings

Some settings might be stored in hidden files or directories other than `.config`. Check and rename other potential directories:

```bash
mv ~/.local ~/.local_backup
mv ~/.cache ~/.cache_backup
```

### Step 6: Reinstall or Reset Desktop Environment

If none of the above steps resolve the issue, you can try reinstalling or resetting the desktop environment:

```bash
sudo apt install --reinstall ubuntu-desktop
```

or reset GNOME settings:

```bash
dconf reset -f /org/gnome/
```

### Step 7: Migrate to the New User

If the issue persists and you cannot identify the problematic configuration, you may consider migrating your data to the new user account (`testuser`). You can transfer your files, and settings, and reconfigure your applications manually.

### Data Migration Steps:

1. **Copy Data**:
   - Copy your important data from your old user directory to the new user directory:

     ```bash
     cp -r /home/original_user/Documents /home/testuser/
     cp -r /home/original_user/Pictures /home/testuser/
     cp -r /home/original_user/Downloads /home/testuser/
     # Repeat for other important directories
     ```

2. **Change Ownership**:
   - Make sure the new user owns the copied files:

     ```bash
     sudo chown -R testuser:testuser /home/testuser/Documents
     sudo chown -R testuser:testuser /home/testuser/Pictures
     sudo chown -R testuser:testuser /home/testuser/Downloads
     # Repeat for other important directories
     ```

3. **Reconfigure Applications**:
   - Log in as `testuser` and reconfigure your applications and settings as needed.

By following these steps, you should be able to identify and resolve the display issue specific to your original user account.