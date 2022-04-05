# linux_kernel_development

##### large file git push issue

用 git log 察看紀錄，並用 git reset XXX 恢復到沒有 commit large file 那次

```
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
git lfs install
git lfs track "*.tar.xz"
git add .gitattributes
git add file.tar.xz
git commit -m "Add large file"
git push
```
