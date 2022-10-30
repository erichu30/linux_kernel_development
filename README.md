# linux_kernel_development



## large file git push issue

use ```git log``` to check the record and use ```git reset XXX``` to recover to the commit before submitting large file

```
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
git lfs install
git lfs track "*.tar.xz"
git add .gitattributes
git add file.tar.xz
git commit -m "Add large file"
git push
```
