SRC_URI = "file://${WORKDIR}/source.tar.gz"
S = "${WORKDIR}/source"

python do_prepare_local_sources() {
    import tarfile, os

    local_src_dir = d.getVar('LOCAL_SRC_DIR')
    tar_path = os.path.join(d.getVar('WORKDIR'), "source.tar.gz")

    with tarfile.open(tar_path, "w:gz") as tar:
        tar.add(local_src_dir, arcname="source")
}

addtask do_prepare_local_sources before do_fetch
