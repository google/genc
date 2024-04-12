# GenC Worker Example

This directory contains example worker server and client binaries that can be
used to experiment with distributed workloads.

*   The **server** binary hosts the default executor stack (the same as what we
    use in the tutorials) behind a gRPC endpoint. The gRPC endpoint hosts the
    [`Executor` service](../../../proto/v0/executor.proto), a gRPC equivalent
    of the [C++ `Executor` interface](../../runtime/executor.h). It can act as
    an execution backend for a remote client (e.g., a client deployed on another
    network, or on a mobile device).

*   The **client** binary can be used to test the communication with the server
    above. It takes IR as a command-line argument, and executes it by issuing
    requests over gRPC to the remote `Executor` service backend. This
    functionality is also available outside this example client binary, as a C++
    component ([`RemoteExecutor`](../../runtime/remote_executor.h)) that you can
    use to setup custom runtime environments.

Here's what an example session, with the client relaying requests to a remote
backend, might look like.

1.  First, create a VM on GCP. Make sure that ingress traffic port 443 (HTTPS)
    is permitted by the firewall policies setup in your organization.

2.  Next, deploy and run the server binary on the VM. For interactive uses, and
    to experiment with various settings, you might find it convenient to setup
    the same build environment as what you use for development, and start the
    binary manually, e.g., as follows.

    ```
    bazel run genc/cc/examples/worker:server -- --port=443 \
      --key=/tmp/key.pem --cert=/tmp/cert.pem
    ```

    You can obtain the private key `key` and the self-signed certificate `cert`
    files, used to establish secure SSL/TSL communication, e.g., by running the
    following command:

    ```
    openssl req -x509 -newkey rsa:4096 -keyout /tmp/key.pem -out /tmp/cert.pem \
      -sha256 -days 3650 -nodes
    ```

    Alternatively, you can build and deploy a pre-packaged docker image (see the
    included `Dockerfile` with an example setup), e.g., as follows. First, you
    need to build the docker image. Make sure that the required key and cert
    files are in the directory that contains the `Dockerfile` where you will be
    building the image (or run the above `openssl` command to generate them in
    that directory), so they can be picked up and packaged with the image during
    the build.

    ```
    cd genc/cc/examples/worker
    docker build -t worker .
    ```

    Once built, you can run the image and publish the port it listes on on the
    host machine, e.g., as follows:

    ```
    docker run -p 443 worker:latest
    ```
    Keep in mind that the simple setup, as described here, does not include any
    form of access control. Thus, if you will be hosting the worker on an
    address that's accessible to other parties, you will need to expand on this
    to protect against unauthorized access. The setup we provided here is
    intended as just an example, for illustration purposes, to get you started.

3.  Finally, on a different machine, run a remote client with a sample IR and
    prompt, e.g., as follows, replacing `SERVER_ADDRESS` with the IP address of
    your server (from the client's perspective), and `SERVER_IDENTITY` by the
    identify you entered when generating the self-signed certificate.

    ```
    bazel run genc/cc/examples/worker:client -- --server=SERVER_ADDRESS:443 \
      --ssl --cert=/tmp/cert.pem --target_override=SERVER_IDENTITY \
      --ir='intrinsic { uri: "model_inference" \
                        static_parameter { str: "test_model" } }' \
      --prompt='Boo!'
    ```

    You can replace the sample IR and prompt with examples from the tutorials.
    Keep in mind that any resources you'll use for execution (e.g., such as the
    Gemma model weights you downloaded) need to be deployed on the machine that
    runs the server binary (not on the client), since that's where your IR will
    be executed.
