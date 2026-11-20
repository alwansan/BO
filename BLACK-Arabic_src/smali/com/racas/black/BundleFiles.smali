.class public final Lcom/racas/black/BundleFiles;
.super Ljava/lang/Object;
.source "BundleFiles.java"


# annotations
.annotation system Ldalvik/annotation/MemberClasses;
    value = {
        Lcom/racas/black/BundleFiles$Status;,
        Lcom/racas/black/BundleFiles$Source;
    }
.end annotation


# direct methods
.method public constructor <init>()V
    .locals 0

    .line 8
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method

.method private static archiveOtherImages(Ljava/io/File;Ljava/io/File;)V
    .locals 5
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 42
    invoke-virtual {p0}, Ljava/io/File;->listFiles()[Ljava/io/File;

    move-result-object p0

    if-eqz p0, :cond_3

    .line 43
    array-length v0, p0

    const/4 v1, 0x0

    :goto_0
    if-ge v1, v0, :cond_2

    aget-object v2, p0, v1

    invoke-virtual {v2}, Ljava/io/File;->isFile()Z

    move-result v3

    if-eqz v3, :cond_1

    invoke-virtual {v2, p1}, Ljava/io/File;->equals(Ljava/lang/Object;)Z

    move-result v3

    if-nez v3, :cond_1

    invoke-virtual {v2}, Ljava/io/File;->getName()Ljava/lang/String;

    move-result-object v3

    sget-object v4, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    invoke-virtual {v3, v4}, Ljava/lang/String;->toLowerCase(Ljava/util/Locale;)Ljava/lang/String;

    move-result-object v3

    const-string v4, ".iso"

    invoke-virtual {v3, v4}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result v3

    if-eqz v3, :cond_1

    .line 44
    invoke-static {v2}, Lcom/racas/black/BundleFiles;->unusedBackup(Ljava/io/File;)Ljava/io/File;

    move-result-object v3

    invoke-virtual {v2, v3}, Ljava/io/File;->renameTo(Ljava/io/File;)Z

    move-result v2

    if-eqz v2, :cond_0

    goto :goto_1

    :cond_0
    new-instance p0, Ljava/io/IOException;

    const-string p1, "\u062a\u0639\u0630\u0631 \u062d\u0641\u0638 \u0646\u0633\u062e\u0629 \u0627\u0644\u0644\u0639\u0628\u0629 \u0627\u0644\u0633\u0627\u0628\u0642\u0629\u061b \u0644\u0645 \u062a\u062d\u0630\u0641 \u0645\u0644\u0641\u0627\u062a\u0643"

    invoke-direct {p0, p1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p0

    .line 43
    :cond_1
    :goto_1
    add-int/lit8 v1, v1, 0x1

    goto :goto_0

    .line 46
    :cond_2
    return-void

    .line 42
    :cond_3
    new-instance p0, Ljava/io/IOException;

    const-string p1, "\u062a\u0639\u0630\u0631 \u0642\u0631\u0627\u0621\u0629 \u0645\u062c\u0644\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-direct {p0, p1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p0
.end method

.method private static digest(Ljava/io/File;Lcom/racas/black/BundleFiles$Status;)Ljava/lang/String;
    .locals 13
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 21
    invoke-static {}, Lcom/racas/black/BundleFiles;->hash()Ljava/security/MessageDigest;

    move-result-object v0

    const/high16 v1, 0x100000

    new-array v1, v1, [B

    .line 22
    new-instance v2, Ljava/io/FileInputStream;

    invoke-direct {v2, p0}, Ljava/io/FileInputStream;-><init>(Ljava/io/File;)V

    const-wide/16 v3, 0x0

    .line 23
    :goto_0
    :try_start_0
    invoke-virtual {v2, v1}, Ljava/io/InputStream;->read([B)I

    move-result v5

    const/4 v6, -0x1

    if-eq v5, v6, :cond_0

    const/4 v6, 0x0

    invoke-virtual {v0, v1, v6, v5}, Ljava/security/MessageDigest;->update([BII)V

    int-to-long v5, v5

    add-long/2addr v3, v5

    const-string v8, "\u062c\u0627\u0631\u064d \u0641\u062d\u0635 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-virtual {p0}, Ljava/io/File;->length()J

    move-result-wide v11

    move-object v7, p1

    move-wide v9, v3

    invoke-interface/range {v7 .. v12}, Lcom/racas/black/BundleFiles$Status;->update(Ljava/lang/String;JJ)V
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    goto :goto_0

    .line 24
    :cond_0
    invoke-virtual {v2}, Ljava/io/InputStream;->close()V

    .line 25
    invoke-virtual {v0}, Ljava/security/MessageDigest;->digest()[B

    move-result-object p0

    invoke-static {p0}, Lcom/racas/black/BundleFiles;->hex([B)Ljava/lang/String;

    move-result-object p0

    return-object p0

    .line 22
    :catchall_0
    move-exception p0

    :try_start_1
    invoke-virtual {v2}, Ljava/io/InputStream;->close()V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception p1

    invoke-virtual {p0, p1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_1
    throw p0
.end method

.method private static hash()Ljava/security/MessageDigest;
    .locals 3
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 17
    :try_start_0
    const-string v0, "SHA-256"

    invoke-static {v0}, Ljava/security/MessageDigest;->getInstance(Ljava/lang/String;)Ljava/security/MessageDigest;

    move-result-object v0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    return-object v0

    .line 18
    :catch_0
    move-exception v0

    new-instance v1, Ljava/io/IOException;

    const-string v2, "\u062a\u0639\u0630\u0631 \u0628\u062f\u0621 \u0627\u0644\u062a\u062d\u0642\u0642 \u0645\u0646 \u0627\u0644\u0645\u0644\u0641"

    invoke-direct {v1, v2, v0}, Ljava/io/IOException;-><init>(Ljava/lang/String;Ljava/lang/Throwable;)V

    throw v1
.end method

.method private static hex([B)Ljava/lang/String;
    .locals 6

    .line 12
    new-instance v0, Ljava/lang/StringBuilder;

    invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V

    .line 13
    array-length v1, p0

    const/4 v2, 0x0

    :goto_0
    if-ge v2, v1, :cond_0

    aget-byte v3, p0, v2

    sget-object v4, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    and-int/lit16 v3, v3, 0xff

    invoke-static {v3}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;

    move-result-object v3

    filled-new-array {v3}, [Ljava/lang/Object;

    move-result-object v3

    const-string v5, "%02x"

    invoke-static {v4, v5, v3}, Ljava/lang/String;->format(Ljava/util/Locale;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v0, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    add-int/lit8 v2, v2, 0x1

    goto :goto_0

    .line 14
    :cond_0
    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method private static mark(Ljava/io/File;Ljava/io/File;Ljava/lang/String;)V
    .locals 2
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 48
    invoke-virtual {p0}, Ljava/io/File;->getParentFile()Ljava/io/File;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/File;->isDirectory()Z

    move-result v1

    if-nez v1, :cond_1

    invoke-virtual {v0}, Ljava/io/File;->mkdirs()Z

    move-result v0

    if-eqz v0, :cond_0

    goto :goto_0

    :cond_0
    new-instance p0, Ljava/io/IOException;

    const-string p1, "\u062a\u0639\u0630\u0631 \u062d\u0641\u0638 \u0646\u062a\u064a\u062c\u0629 \u0627\u0644\u0641\u062d\u0635"

    invoke-direct {p0, p1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p0

    .line 50
    :cond_1
    :goto_0
    new-instance v0, Ljava/io/FileOutputStream;

    invoke-direct {v0, p0}, Ljava/io/FileOutputStream;-><init>(Ljava/io/File;)V

    :try_start_0
    invoke-static {p1, p2}, Lcom/racas/black/BundleFiles;->stamp(Ljava/io/File;Ljava/lang/String;)Ljava/lang/String;

    move-result-object p0

    const-string p1, "UTF-8"

    invoke-virtual {p0, p1}, Ljava/lang/String;->getBytes(Ljava/lang/String;)[B

    move-result-object p0

    invoke-virtual {v0, p0}, Ljava/io/FileOutputStream;->write([B)V

    invoke-virtual {v0}, Ljava/io/FileOutputStream;->getFD()Ljava/io/FileDescriptor;

    move-result-object p0

    invoke-virtual {p0}, Ljava/io/FileDescriptor;->sync()V
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    invoke-virtual {v0}, Ljava/io/FileOutputStream;->close()V

    .line 51
    return-void

    .line 50
    :catchall_0
    move-exception p0

    :try_start_1
    invoke-virtual {v0}, Ljava/io/FileOutputStream;->close()V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception p1

    invoke-virtual {p0, p1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_1
    throw p0
.end method

.method public static declared-synchronized prepare(Ljava/io/File;Ljava/io/File;Ljava/lang/String;JLjava/lang/String;Lcom/racas/black/BundleFiles$Source;Lcom/racas/black/BundleFiles$Status;)Ljava/io/File;
    .locals 19
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    move-object/from16 v0, p0

    move-object/from16 v1, p1

    move-object/from16 v2, p2

    move-object/from16 v8, p5

    const-class v9, Lcom/racas/black/BundleFiles;

    monitor-enter v9

    .line 53
    :try_start_0
    invoke-virtual/range {p0 .. p0}, Ljava/io/File;->isDirectory()Z

    move-result v3

    if-nez v3, :cond_1

    invoke-virtual/range {p0 .. p0}, Ljava/io/File;->mkdirs()Z

    move-result v3

    if-eqz v3, :cond_0

    goto :goto_0

    :cond_0
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u062a\u0639\u0630\u0631 \u0625\u0646\u0634\u0627\u0621 \u0645\u062c\u0644\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0

    .line 54
    :cond_1
    :goto_0
    new-instance v10, Ljava/io/File;

    invoke-direct {v10, v0, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    new-instance v11, Ljava/io/File;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v3, ".installing"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-direct {v11, v0, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 55
    invoke-virtual {v10}, Ljava/io/File;->getCanonicalFile()Ljava/io/File;

    move-result-object v2

    invoke-virtual {v2}, Ljava/io/File;->getParentFile()Ljava/io/File;

    move-result-object v2

    invoke-virtual/range {p0 .. p0}, Ljava/io/File;->getCanonicalFile()Ljava/io/File;

    move-result-object v3

    invoke-virtual {v2, v3}, Ljava/io/File;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-eqz v2, :cond_10

    invoke-virtual {v11}, Ljava/io/File;->getCanonicalFile()Ljava/io/File;

    move-result-object v2

    invoke-virtual {v2}, Ljava/io/File;->getParentFile()Ljava/io/File;

    move-result-object v2

    invoke-virtual/range {p0 .. p0}, Ljava/io/File;->getCanonicalFile()Ljava/io/File;

    move-result-object v3

    invoke-virtual {v2, v3}, Ljava/io/File;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-eqz v2, :cond_10

    .line 56
    invoke-virtual {v10}, Ljava/io/File;->isFile()Z

    move-result v2

    if-eqz v2, :cond_3

    invoke-virtual {v10}, Ljava/io/File;->length()J

    move-result-wide v2

    cmp-long v2, v2, p3

    if-nez v2, :cond_3

    invoke-static/range {p1 .. p1}, Lcom/racas/black/BundleFiles;->read(Ljava/io/File;)Ljava/lang/String;

    move-result-object v2

    invoke-static {v10, v8}, Lcom/racas/black/BundleFiles;->stamp(Ljava/io/File;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v2, v3}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-nez v2, :cond_2

    move-object/from16 v12, p7

    invoke-static {v10, v12}, Lcom/racas/black/BundleFiles;->digest(Ljava/io/File;Lcom/racas/black/BundleFiles$Status;)Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v2, v8}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-eqz v2, :cond_4

    .line 57
    :cond_2
    invoke-static {v0, v10}, Lcom/racas/black/BundleFiles;->archiveOtherImages(Ljava/io/File;Ljava/io/File;)V

    invoke-static {v1, v10, v8}, Lcom/racas/black/BundleFiles;->mark(Ljava/io/File;Ljava/io/File;Ljava/lang/String;)V
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_a

    monitor-exit v9

    return-object v10

    .line 56
    :cond_3
    move-object/from16 v12, p7

    .line 59
    :cond_4
    :try_start_1
    invoke-virtual/range {p0 .. p0}, Ljava/io/File;->getUsableSpace()J

    move-result-wide v2

    const-wide/32 v4, 0x2000000

    add-long v4, p3, v4

    cmp-long v2, v2, v4

    if-ltz v2, :cond_f

    .line 60
    invoke-static {}, Lcom/racas/black/BundleFiles;->hash()Ljava/security/MessageDigest;

    move-result-object v13
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_a

    .line 62
    :try_start_2
    invoke-interface/range {p6 .. p6}, Lcom/racas/black/BundleFiles$Source;->open()Ljava/io/InputStream;

    move-result-object v15
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_9

    :try_start_3
    new-instance v6, Ljava/io/FileOutputStream;

    invoke-direct {v6, v11}, Ljava/io/FileOutputStream;-><init>(Ljava/io/File;)V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_6

    .line 63
    const/high16 v2, 0x100000

    :try_start_4
    new-array v7, v2, [B

    const-wide/16 v2, 0x0

    .line 64
    :goto_1
    invoke-virtual {v15, v7}, Ljava/io/InputStream;->read([B)I

    move-result v4
    :try_end_4
    .catchall {:try_start_4 .. :try_end_4} :catchall_3

    const/4 v5, -0x1

    if-eq v4, v5, :cond_6

    .line 65
    move-object/from16 p6, v15

    int-to-long v14, v4

    add-long/2addr v14, v2

    cmp-long v2, v14, p3

    if-gtz v2, :cond_5

    .line 66
    const/4 v5, 0x0

    :try_start_5
    invoke-virtual {v6, v7, v5, v4}, Ljava/io/FileOutputStream;->write([BII)V

    invoke-virtual {v13, v7, v5, v4}, Ljava/security/MessageDigest;->update([BII)V

    const-string v3, "\u062c\u0627\u0631\u064d \u062a\u062c\u0647\u064a\u0632 \u0627\u0644\u0644\u0639\u0628\u0629 \u2014 \u064a\u062a\u0645 \u0645\u0631\u0629 \u0648\u0627\u062d\u062f\u0629"
    :try_end_5
    .catchall {:try_start_5 .. :try_end_5} :catchall_0

    move-object/from16 v2, p7

    move/from16 v16, v5

    move-wide v4, v14

    move-object/from16 v17, v6

    move-object/from16 v18, v7

    move-wide/from16 v6, p3

    :try_start_6
    invoke-interface/range {v2 .. v7}, Lcom/racas/black/BundleFiles$Status;->update(Ljava/lang/String;JJ)V

    move-wide v2, v14

    move-object/from16 v6, v17

    move-object/from16 v7, v18

    move-object/from16 v15, p6

    goto :goto_1

    .line 62
    :catchall_0
    move-exception v0

    move/from16 v16, v5

    move-object/from16 v17, v6

    goto/16 :goto_3

    .line 65
    :cond_5
    move-object/from16 v17, v6

    const/16 v16, 0x0

    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u062d\u062c\u0645 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629 \u063a\u064a\u0631 \u0645\u0637\u0627\u0628\u0642"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0

    .line 68
    :cond_6
    move-object/from16 v17, v6

    move-object/from16 p6, v15

    const/16 v16, 0x0

    cmp-long v2, v2, p3

    if-nez v2, :cond_c

    invoke-virtual {v13}, Ljava/security/MessageDigest;->digest()[B

    move-result-object v2

    invoke-static {v2}, Lcom/racas/black/BundleFiles;->hex([B)Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v2, v8}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-eqz v2, :cond_c

    .line 69
    invoke-virtual/range {v17 .. v17}, Ljava/io/FileOutputStream;->getFD()Ljava/io/FileDescriptor;

    move-result-object v2

    invoke-virtual {v2}, Ljava/io/FileDescriptor;->sync()V
    :try_end_6
    .catchall {:try_start_6 .. :try_end_6} :catchall_2

    .line 70
    :try_start_7
    invoke-virtual/range {v17 .. v17}, Ljava/io/FileOutputStream;->close()V
    :try_end_7
    .catchall {:try_start_7 .. :try_end_7} :catchall_5

    if-eqz p6, :cond_7

    :try_start_8
    invoke-virtual/range {p6 .. p6}, Ljava/io/InputStream;->close()V

    .line 71
    :cond_7
    invoke-virtual {v10}, Ljava/io/File;->exists()Z

    move-result v2

    if-eqz v2, :cond_9

    invoke-static {v10}, Lcom/racas/black/BundleFiles;->unusedBackup(Ljava/io/File;)Ljava/io/File;

    move-result-object v2

    invoke-virtual {v10, v2}, Ljava/io/File;->renameTo(Ljava/io/File;)Z

    move-result v3

    if-eqz v3, :cond_8

    goto :goto_2

    :cond_8
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u062a\u0639\u0630\u0631 \u062d\u0641\u0638 \u0646\u0633\u062e\u0629 \u0627\u0644\u0645\u0644\u0641 \u0627\u0644\u0633\u0627\u0628\u0642"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0

    :cond_9
    const/4 v2, 0x0

    .line 72
    :goto_2
    invoke-virtual {v11, v10}, Ljava/io/File;->renameTo(Ljava/io/File;)Z

    move-result v3

    if-nez v3, :cond_b

    .line 73
    if-eqz v2, :cond_a

    invoke-virtual {v2, v10}, Ljava/io/File;->renameTo(Ljava/io/File;)Z

    move-result v0

    if-nez v0, :cond_a

    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u062a\u0639\u0630\u0631 \u0646\u0642\u0644 \u0627\u0644\u0645\u0644\u0641\u061b \u0627\u0644\u0646\u0633\u062e\u0629 \u0627\u0644\u0633\u0627\u0628\u0642\u0629 \u0645\u062d\u0641\u0648\u0638\u0629 \u0628\u062c\u0627\u0646\u0628\u0647"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0

    .line 74
    :cond_a
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u062a\u0639\u0630\u0631 \u0648\u0636\u0639 \u0645\u0644\u0641 \u0627\u0644\u0644\u0639\u0628\u0629 \u0641\u064a \u0645\u0643\u0627\u0646\u0647"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0
    :try_end_8
    .catchall {:try_start_8 .. :try_end_8} :catchall_8

    .line 76
    :cond_b
    const/4 v14, 0x1

    :try_start_9
    invoke-static {v0, v10}, Lcom/racas/black/BundleFiles;->archiveOtherImages(Ljava/io/File;Ljava/io/File;)V

    invoke-static {v1, v10, v8}, Lcom/racas/black/BundleFiles;->mark(Ljava/io/File;Ljava/io/File;Ljava/lang/String;)V
    :try_end_9
    .catchall {:try_start_9 .. :try_end_9} :catchall_1

    .line 78
    nop

    .line 76
    monitor-exit v9

    return-object v10

    .line 78
    :catchall_1
    move-exception v0

    goto :goto_8

    .line 68
    :cond_c
    :try_start_a
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u0641\u0634\u0644 \u0627\u0644\u062a\u062d\u0642\u0642 \u0645\u0646 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629. \u0623\u0639\u062f \u062a\u0646\u0632\u064a\u0644 \u0645\u0644\u0641 \u0627\u0644\u062a\u0637\u0628\u064a\u0642 \u0643\u0627\u0645\u0644\u0627\u064b"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0
    :try_end_a
    .catchall {:try_start_a .. :try_end_a} :catchall_2

    .line 62
    :catchall_2
    move-exception v0

    goto :goto_3

    :catchall_3
    move-exception v0

    move-object/from16 v17, v6

    move-object/from16 p6, v15

    const/16 v16, 0x0

    :goto_3
    move-object v1, v0

    :try_start_b
    invoke-virtual/range {v17 .. v17}, Ljava/io/FileOutputStream;->close()V
    :try_end_b
    .catchall {:try_start_b .. :try_end_b} :catchall_4

    goto :goto_4

    :catchall_4
    move-exception v0

    move-object v2, v0

    :try_start_c
    invoke-virtual {v1, v2}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_4
    throw v1
    :try_end_c
    .catchall {:try_start_c .. :try_end_c} :catchall_5

    :catchall_5
    move-exception v0

    goto :goto_5

    :catchall_6
    move-exception v0

    move-object/from16 p6, v15

    const/16 v16, 0x0

    :goto_5
    move-object v1, v0

    if-eqz p6, :cond_d

    :try_start_d
    invoke-virtual/range {p6 .. p6}, Ljava/io/InputStream;->close()V
    :try_end_d
    .catchall {:try_start_d .. :try_end_d} :catchall_7

    goto :goto_6

    :catchall_7
    move-exception v0

    move-object v2, v0

    :try_start_e
    invoke-virtual {v1, v2}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_d
    :goto_6
    throw v1
    :try_end_e
    .catchall {:try_start_e .. :try_end_e} :catchall_8

    .line 78
    :catchall_8
    move-exception v0

    goto :goto_7

    :catchall_9
    move-exception v0

    const/16 v16, 0x0

    :goto_7
    move/from16 v14, v16

    :goto_8
    if-nez v14, :cond_e

    :try_start_f
    invoke-virtual {v11}, Ljava/io/File;->exists()Z

    move-result v1

    if-eqz v1, :cond_e

    invoke-virtual {v11}, Ljava/io/File;->delete()Z

    move-result v1

    if-nez v1, :cond_e

    invoke-virtual {v11}, Ljava/io/File;->deleteOnExit()V

    .line 79
    :cond_e
    throw v0

    .line 59
    :cond_f
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u0627\u0644\u0645\u0633\u0627\u062d\u0629 \u063a\u064a\u0631 \u0643\u0627\u0641\u064a\u0629. \u0648\u0641\u0651\u0631 \u063a\u064a\u063a\u0627\u0628\u0627\u064a\u062a\u064a\u0646 \u0625\u0636\u0627\u0641\u064a\u062a\u064a\u0646 \u062a\u0642\u0631\u064a\u0628\u0627\u064b \u062b\u0645 \u0623\u0639\u062f \u0627\u0644\u0645\u062d\u0627\u0648\u0644\u0629"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0

    .line 55
    :cond_10
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u0645\u0633\u0627\u0631 \u0627\u0644\u0644\u0639\u0628\u0629 \u063a\u064a\u0631 \u0635\u0627\u0644\u062d"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0
    :try_end_f
    .catchall {:try_start_f .. :try_end_f} :catchall_a

    .line 52
    :catchall_a
    move-exception v0

    monitor-exit v9

    throw v0
.end method

.method private static read(Ljava/io/File;)Ljava/lang/String;
    .locals 5

    .line 30
    const-string v0, ""

    :try_start_0
    invoke-virtual {p0}, Ljava/io/File;->length()J

    move-result-wide v1

    const-wide/16 v3, 0x1000

    cmp-long v1, v1, v3

    if-lez v1, :cond_0

    return-object v0

    .line 31
    :cond_0
    new-instance v1, Ljava/io/ByteArrayOutputStream;

    invoke-direct {v1}, Ljava/io/ByteArrayOutputStream;-><init>()V

    .line 32
    new-instance v2, Ljava/io/FileInputStream;

    invoke-direct {v2, p0}, Ljava/io/FileInputStream;-><init>(Ljava/io/File;)V
    :try_end_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_0

    const/16 p0, 0x400

    :try_start_1
    new-array p0, p0, [B

    :goto_0
    invoke-virtual {v2, p0}, Ljava/io/InputStream;->read([B)I

    move-result v3

    const/4 v4, -0x1

    if-eq v3, v4, :cond_1

    const/4 v4, 0x0

    invoke-virtual {v1, p0, v4, v3}, Ljava/io/ByteArrayOutputStream;->write([BII)V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    goto :goto_0

    :cond_1
    :try_start_2
    invoke-virtual {v2}, Ljava/io/InputStream;->close()V

    .line 33
    new-instance p0, Ljava/lang/String;

    invoke-virtual {v1}, Ljava/io/ByteArrayOutputStream;->toByteArray()[B

    move-result-object v1

    const-string v2, "UTF-8"

    invoke-direct {p0, v1, v2}, Ljava/lang/String;-><init>([BLjava/lang/String;)V
    :try_end_2
    .catch Ljava/io/IOException; {:try_start_2 .. :try_end_2} :catch_0

    return-object p0

    .line 32
    :catchall_0
    move-exception p0

    :try_start_3
    invoke-virtual {v2}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception v1

    :try_start_4
    invoke-virtual {p0, v1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_1
    throw p0
    :try_end_4
    .catch Ljava/io/IOException; {:try_start_4 .. :try_end_4} :catch_0

    .line 34
    :catch_0
    move-exception p0

    return-object v0
.end method

.method private static stamp(Ljava/io/File;Ljava/lang/String;)Ljava/lang/String;
    .locals 3

    .line 27
    new-instance v0, Ljava/lang/StringBuilder;

    invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {v0, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string v0, "\n"

    invoke-virtual {p1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p0}, Ljava/io/File;->length()J

    move-result-wide v1

    invoke-virtual {p1, v1, v2}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p0}, Ljava/io/File;->lastModified()J

    move-result-wide v1

    invoke-virtual {p1, v1, v2}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p0}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object p0

    invoke-virtual {p1, p0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method private static unusedBackup(Ljava/io/File;)Ljava/io/File;
    .locals 4

    .line 37
    const/4 v0, 0x0

    .line 38
    :goto_0
    new-instance v1, Ljava/io/File;

    new-instance v2, Ljava/lang/StringBuilder;

    invoke-direct {v2}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {p0}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v3, ".previous-bundle-"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    add-int/lit8 v3, v0, 0x1

    invoke-virtual {v2, v0}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-direct {v1, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    invoke-virtual {v1}, Ljava/io/File;->exists()Z

    move-result v0

    if-nez v0, :cond_0

    .line 39
    return-object v1

    .line 38
    :cond_0
    move v0, v3

    goto :goto_0
.end method
