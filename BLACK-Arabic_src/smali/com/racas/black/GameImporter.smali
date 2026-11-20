.class final Lcom/racas/black/GameImporter;
.super Ljava/lang/Object;
.source "GameImporter.java"


# static fields
.field private static final BUFFER:I = 0x400000

.field private static final MAGIC:[B

.field private static final PARTITIONS:[J

.field static final REQUEST:I = 0x8a11

.field private static final TAG:Ljava/lang/String; = "BlackImport"


# instance fields
.field private final activity:Landroid/app/Activity;

.field private volatile cancelled:Z

.field private final main:Landroid/os/Handler;


# direct methods
.method public static synthetic $r8$lambda$368ZxYc0gp8T1wcsBV8DEQpTOtg(Lcom/racas/black/GameImporter;Landroid/app/AlertDialog;ZLjava/io/File;Ljava/lang/String;Ljava/util/function/Consumer;Ljava/lang/String;)V
    .locals 0

    invoke-direct/range {p0 .. p6}, Lcom/racas/black/GameImporter;->lambda$copy$6(Landroid/app/AlertDialog;ZLjava/io/File;Ljava/lang/String;Ljava/util/function/Consumer;Ljava/lang/String;)V

    return-void
.end method

.method public static synthetic $r8$lambda$4h9e0X6baDSNHmIxNHvuAxs3mSo(Lcom/racas/black/GameImporter;Landroid/net/Uri;Ljava/lang/String;Ljava/io/File;Landroid/widget/ProgressBar;Landroid/widget/TextView;JLjava/io/File;Landroid/app/AlertDialog;Ljava/util/function/Consumer;)V
    .locals 0

    invoke-direct/range {p0 .. p10}, Lcom/racas/black/GameImporter;->lambda$copy$7(Landroid/net/Uri;Ljava/lang/String;Ljava/io/File;Landroid/widget/ProgressBar;Landroid/widget/TextView;JLjava/io/File;Landroid/app/AlertDialog;Ljava/util/function/Consumer;)V

    return-void
.end method

.method public static synthetic $r8$lambda$Lvxfe0x7uoLRo8zdaCTUl_Tn91E(Lcom/racas/black/GameImporter;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/GameImporter;->lambda$copy$2(Landroid/content/DialogInterface;I)V

    return-void
.end method

.method static constructor <clinit>()V
    .locals 1

    .line 33
    const-string v0, "MICROSOFT*XBOX*MEDIA"

    invoke-virtual {v0}, Ljava/lang/String;->getBytes()[B

    move-result-object v0

    sput-object v0, Lcom/racas/black/GameImporter;->MAGIC:[B

    const/4 v0, 0x4

    .line 35
    new-array v0, v0, [J

    fill-array-data v0, :array_0

    sput-object v0, Lcom/racas/black/GameImporter;->PARTITIONS:[J

    return-void

    nop

    :array_0
    .array-data 8
        0x0
        0x18300000
        0xfd90000
        0x2080000
    .end array-data
.end method

.method constructor <init>(Landroid/app/Activity;)V
    .locals 2

    .line 42
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    .line 39
    new-instance v0, Landroid/os/Handler;

    invoke-static {}, Landroid/os/Looper;->getMainLooper()Landroid/os/Looper;

    move-result-object v1

    invoke-direct {v0, v1}, Landroid/os/Handler;-><init>(Landroid/os/Looper;)V

    iput-object v0, p0, Lcom/racas/black/GameImporter;->main:Landroid/os/Handler;

    .line 43
    iput-object p1, p0, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    return-void
.end method

.method private displayName(Landroid/net/Uri;)Ljava/lang/String;
    .locals 7

    .line 172
    :try_start_0
    iget-object p0, p0, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-virtual {p0}, Landroid/app/Activity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v0

    const/4 p0, 0x1

    new-array v2, p0, [Ljava/lang/String;

    const-string p0, "_display_name"

    const/4 v6, 0x0

    aput-object p0, v2, v6

    const/4 v4, 0x0

    const/4 v5, 0x0

    const/4 v3, 0x0

    move-object v1, p1

    invoke-virtual/range {v0 .. v5}, Landroid/content/ContentResolver;->query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;

    move-result-object p0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    if-eqz p0, :cond_2

    .line 173
    :try_start_1
    invoke-interface {p0}, Landroid/database/Cursor;->moveToFirst()Z

    move-result v0

    if-eqz v0, :cond_2

    invoke-interface {p0, v6}, Landroid/database/Cursor;->isNull(I)Z

    move-result v0

    if-nez v0, :cond_2

    new-instance v0, Ljava/io/File;

    invoke-interface {p0, v6}, Landroid/database/Cursor;->getString(I)Ljava/lang/String;

    move-result-object v1

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    invoke-virtual {v0}, Ljava/io/File;->getName()Ljava/lang/String;

    move-result-object v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    if-eqz p0, :cond_0

    .line 174
    :try_start_2
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    :cond_0
    return-object v0

    :catchall_0
    move-exception v0

    if-eqz p0, :cond_1

    .line 172
    :try_start_3
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception p0

    :try_start_4
    invoke-virtual {v0, p0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_1
    :goto_0
    throw v0

    :cond_2
    if-eqz p0, :cond_3

    .line 174
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    .line 177
    :catch_0
    :cond_3
    invoke-virtual {p1}, Landroid/net/Uri;->getLastPathSegment()Ljava/lang/String;

    move-result-object p0

    if-nez p0, :cond_4

    const/4 p0, 0x0

    goto :goto_1

    .line 178
    :cond_4
    new-instance p1, Ljava/io/File;

    invoke-direct {p1, p0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    invoke-virtual {p1}, Ljava/io/File;->getName()Ljava/lang/String;

    move-result-object p0

    :goto_1
    return-object p0
.end method

.method private static isXboxImage(Ljava/nio/channels/FileChannel;)Ljava/lang/Boolean;
    .locals 9

    .line 158
    sget-object v0, Lcom/racas/black/GameImporter;->MAGIC:[B

    array-length v0, v0

    invoke-static {v0}, Ljava/nio/ByteBuffer;->allocate(I)Ljava/nio/ByteBuffer;

    move-result-object v0

    .line 160
    :try_start_0
    sget-object v1, Lcom/racas/black/GameImporter;->PARTITIONS:[J

    array-length v2, v1

    const/4 v3, 0x0

    move v4, v3

    :goto_0
    if-ge v4, v2, :cond_1

    aget-wide v5, v1, v4

    .line 161
    invoke-virtual {v0}, Ljava/nio/ByteBuffer;->clear()Ljava/nio/Buffer;

    const-wide/32 v7, 0x10000

    add-long/2addr v5, v7

    .line 162
    invoke-virtual {p0, v0, v5, v6}, Ljava/nio/channels/FileChannel;->read(Ljava/nio/ByteBuffer;J)I

    move-result v5

    sget-object v6, Lcom/racas/black/GameImporter;->MAGIC:[B

    array-length v7, v6

    if-ne v5, v7, :cond_0

    invoke-virtual {v0}, Ljava/nio/ByteBuffer;->array()[B

    move-result-object v5

    invoke-static {v5, v6}, Ljava/util/Arrays;->equals([B[B)Z

    move-result v5

    if-eqz v5, :cond_0

    const/4 p0, 0x1

    invoke-static {p0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object p0

    return-object p0

    :cond_0
    add-int/lit8 v4, v4, 0x1

    goto :goto_0

    .line 164
    :cond_1
    invoke-static {v3}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object p0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    return-object p0

    :catch_0
    move-exception p0

    .line 166
    new-instance v0, Ljava/lang/StringBuilder;

    const-string v1, "image not checked, no seeking: "

    invoke-direct {v0, v1}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v0, p0}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    const-string v0, "BlackImport"

    invoke-static {v0, p0}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    const/4 p0, 0x0

    return-object p0
.end method

.method static synthetic lambda$copy$0(Ljava/util/function/Consumer;)V
    .locals 1

    const/4 v0, 0x0

    .line 65
    invoke-static {v0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object v0

    invoke-interface {p0, v0}, Ljava/util/function/Consumer;->accept(Ljava/lang/Object;)V

    return-void
.end method

.method static synthetic lambda$copy$1(Ljava/util/function/Consumer;)V
    .locals 1

    const/4 v0, 0x0

    .line 71
    invoke-static {v0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object v0

    invoke-interface {p0, v0}, Ljava/util/function/Consumer;->accept(Ljava/lang/Object;)V

    return-void
.end method

.method private synthetic lambda$copy$2(Landroid/content/DialogInterface;I)V
    .locals 0

    const/4 p1, 0x1

    .line 91
    iput-boolean p1, p0, Lcom/racas/black/GameImporter;->cancelled:Z

    return-void
.end method

.method static synthetic lambda$copy$3(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V
    .locals 0

    .line 115
    invoke-static/range {p0 .. p5}, Lcom/racas/black/GameImporter;->show(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V

    return-void
.end method

.method static synthetic lambda$copy$4(Ljava/util/function/Consumer;)V
    .locals 1

    const/4 v0, 0x1

    .line 136
    invoke-static {v0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object v0

    invoke-interface {p0, v0}, Ljava/util/function/Consumer;->accept(Ljava/lang/Object;)V

    return-void
.end method

.method static synthetic lambda$copy$5(Ljava/util/function/Consumer;)V
    .locals 1

    const/4 v0, 0x0

    .line 138
    invoke-static {v0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object v0

    invoke-interface {p0, v0}, Ljava/util/function/Consumer;->accept(Ljava/lang/Object;)V

    return-void
.end method

.method private synthetic lambda$copy$6(Landroid/app/AlertDialog;ZLjava/io/File;Ljava/lang/String;Ljava/util/function/Consumer;Ljava/lang/String;)V
    .locals 0

    .line 133
    invoke-virtual {p1}, Landroid/app/AlertDialog;->dismiss()V

    if-eqz p2, :cond_0

    .line 135
    new-instance p1, Ljava/lang/StringBuilder;

    const-string p2, "imported "

    invoke-direct {p1, p2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {p1, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    const-string p2, "BlackImport"

    invoke-static {p2, p1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 136
    new-instance p1, Ljava/lang/StringBuilder;

    invoke-direct {p1}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {p1, p4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string p2, " \u062c\u0627\u0647\u0632. \u0633\u062a\u0628\u062f\u0623 \u0627\u0644\u0644\u0639\u0628\u0629 \u0628\u0639\u062f \u0644\u062d\u0638\u0627\u062a."

    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    new-instance p2, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda6;

    invoke-direct {p2, p5}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda6;-><init>(Ljava/util/function/Consumer;)V

    const-string p3, "\u0627\u0643\u062a\u0645\u0644 \u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-direct {p0, p3, p1, p2}, Lcom/racas/black/GameImporter;->message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V

    goto :goto_0

    :cond_0
    if-eqz p6, :cond_1

    .line 138
    new-instance p1, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda7;

    invoke-direct {p1, p5}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda7;-><init>(Ljava/util/function/Consumer;)V

    const-string p2, "\u062a\u0639\u0630\u0631 \u0627\u0644\u0627\u0633\u062a\u064a\u0631\u0627\u062f"

    invoke-direct {p0, p2, p6, p1}, Lcom/racas/black/GameImporter;->message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V

    goto :goto_0

    :cond_1
    const/4 p0, 0x0

    .line 140
    invoke-static {p0}, Ljava/lang/Boolean;->valueOf(Z)Ljava/lang/Boolean;

    move-result-object p0

    invoke-interface {p5, p0}, Ljava/util/function/Consumer;->accept(Ljava/lang/Object;)V

    :goto_0
    return-void
.end method

.method private synthetic lambda$copy$7(Landroid/net/Uri;Ljava/lang/String;Ljava/io/File;Landroid/widget/ProgressBar;Landroid/widget/TextView;JLjava/io/File;Landroid/app/AlertDialog;Ljava/util/function/Consumer;)V
    .locals 20

    move-object/from16 v2, p0

    move-object/from16 v1, p3

    .line 99
    :try_start_0
    iget-object v0, v2, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-virtual {v0}, Landroid/app/Activity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v0

    const-string v3, "r"

    move-object/from16 v4, p1

    invoke-virtual {v0, v4, v3}, Landroid/content/ContentResolver;->openFileDescriptor(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;

    move-result-object v10
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 100
    :try_start_1
    new-instance v11, Ljava/io/FileInputStream;

    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->getFileDescriptor()Ljava/io/FileDescriptor;

    move-result-object v0

    invoke-direct {v11, v0}, Ljava/io/FileInputStream;-><init>(Ljava/io/FileDescriptor;)V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_9

    .line 101
    :try_start_2
    invoke-virtual {v11}, Ljava/io/FileInputStream;->getChannel()Ljava/nio/channels/FileChannel;

    move-result-object v12
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_7

    .line 102
    :try_start_3
    invoke-static {v12}, Lcom/racas/black/GameImporter;->isXboxImage(Ljava/nio/channels/FileChannel;)Ljava/lang/Boolean;

    move-result-object v0

    sget-object v3, Ljava/lang/Boolean;->FALSE:Ljava/lang/Boolean;
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_5

    if-ne v0, v3, :cond_0

    .line 103
    :try_start_4
    new-instance v0, Ljava/lang/StringBuilder;

    invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V
    :try_end_4
    .catchall {:try_start_4 .. :try_end_4} :catchall_0

    move-object/from16 v13, p2

    :try_start_5
    invoke-virtual {v0, v13}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    const-string v3, " \u0644\u064a\u0633 \u0645\u0644\u0641 \u0642\u0631\u0635 Xbox \u0635\u0627\u0644\u062d\u0627\u064b."

    invoke-virtual {v0, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    goto/16 :goto_4

    :catchall_0
    move-exception v0

    move-object/from16 v13, p2

    goto/16 :goto_7

    :cond_0
    move-object/from16 v13, p2

    .line 105
    new-instance v14, Ljava/io/FileOutputStream;

    invoke-direct {v14, v1}, Ljava/io/FileOutputStream;-><init>(Ljava/io/File;)V
    :try_end_5
    .catchall {:try_start_5 .. :try_end_5} :catchall_5

    :try_start_6
    invoke-virtual {v14}, Ljava/io/FileOutputStream;->getChannel()Ljava/nio/channels/FileChannel;

    move-result-object v15
    :try_end_6
    .catchall {:try_start_6 .. :try_end_6} :catchall_3

    const/high16 v0, 0x400000

    .line 106
    :try_start_7
    invoke-static {v0}, Ljava/nio/ByteBuffer;->allocateDirect(I)Ljava/nio/ByteBuffer;

    move-result-object v0

    const-wide/16 v16, 0x0

    move-wide/from16 v3, v16

    :goto_0
    move-wide v5, v3

    .line 108
    :goto_1
    iget-boolean v7, v2, Lcom/racas/black/GameImporter;->cancelled:Z

    if-nez v7, :cond_3

    invoke-virtual {v12, v0}, Ljava/nio/channels/FileChannel;->read(Ljava/nio/ByteBuffer;)I

    move-result v7

    if-lez v7, :cond_3

    .line 109
    invoke-virtual {v0}, Ljava/nio/ByteBuffer;->flip()Ljava/nio/Buffer;

    move-wide/from16 v18, v3

    .line 110
    :goto_2
    invoke-virtual {v0}, Ljava/nio/ByteBuffer;->hasRemaining()Z

    move-result v3

    if-eqz v3, :cond_1

    invoke-virtual {v15, v0}, Ljava/nio/channels/FileChannel;->write(Ljava/nio/ByteBuffer;)I

    move-result v3

    int-to-long v3, v3

    add-long v18, v18, v3

    goto :goto_2

    .line 111
    :cond_1
    invoke-virtual {v0}, Ljava/nio/ByteBuffer;->clear()Ljava/nio/Buffer;

    sub-long v3, v18, v5

    const-wide/32 v7, 0x1000000

    cmp-long v3, v3, v7

    if-ltz v3, :cond_2

    .line 115
    iget-object v8, v2, Lcom/racas/black/GameImporter;->main:Landroid/os/Handler;

    new-instance v9, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;

    move-object v3, v9

    move-object/from16 v4, p4

    move-object/from16 v5, p5

    move-wide/from16 v6, v18

    move-object/from16 p1, v0

    move-object v0, v8

    move-object v13, v9

    move-wide/from16 v8, p6

    invoke-direct/range {v3 .. v9}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;-><init>(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V

    invoke-virtual {v0, v13}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    move-object/from16 v0, p1

    move-object/from16 v13, p2

    move-wide/from16 v3, v18

    goto :goto_0

    :cond_2
    move-object/from16 v13, p2

    move-wide/from16 v3, v18

    goto :goto_1

    .line 118
    :cond_3
    invoke-virtual {v14}, Ljava/io/FileOutputStream;->getFD()Ljava/io/FileDescriptor;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/FileDescriptor;->sync()V

    .line 119
    iget-boolean v0, v2, Lcom/racas/black/GameImporter;->cancelled:Z

    if-nez v0, :cond_4

    cmp-long v0, p6, v16

    if-lez v0, :cond_4

    cmp-long v0, v3, p6

    if-eqz v0, :cond_4

    const-string v0, "\u062a\u0648\u0642\u0641 \u0627\u0644\u0646\u0633\u062e \u0642\u0628\u0644 \u0627\u0643\u062a\u0645\u0627\u0644\u0647."
    :try_end_7
    .catchall {:try_start_7 .. :try_end_7} :catchall_1

    goto :goto_3

    :cond_4
    const/4 v0, 0x0

    :goto_3
    if-eqz v15, :cond_5

    .line 120
    :try_start_8
    invoke-virtual {v15}, Ljava/nio/channels/FileChannel;->close()V
    :try_end_8
    .catchall {:try_start_8 .. :try_end_8} :catchall_3

    :cond_5
    :try_start_9
    invoke-virtual {v14}, Ljava/io/FileOutputStream;->close()V
    :try_end_9
    .catchall {:try_start_9 .. :try_end_9} :catchall_5

    :goto_4
    if-eqz v12, :cond_6

    .line 122
    :try_start_a
    invoke-virtual {v12}, Ljava/nio/channels/FileChannel;->close()V
    :try_end_a
    .catchall {:try_start_a .. :try_end_a} :catchall_7

    :cond_6
    :try_start_b
    invoke-virtual {v11}, Ljava/io/FileInputStream;->close()V
    :try_end_b
    .catchall {:try_start_b .. :try_end_b} :catchall_9

    if-eqz v10, :cond_a

    :try_start_c
    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->close()V
    :try_end_c
    .catch Ljava/lang/Exception; {:try_start_c .. :try_end_c} :catch_0

    goto/16 :goto_b

    :catchall_1
    move-exception v0

    move-object v3, v0

    if-eqz v15, :cond_7

    .line 105
    :try_start_d
    invoke-virtual {v15}, Ljava/nio/channels/FileChannel;->close()V
    :try_end_d
    .catchall {:try_start_d .. :try_end_d} :catchall_2

    goto :goto_5

    :catchall_2
    move-exception v0

    move-object v4, v0

    :try_start_e
    invoke-virtual {v3, v4}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_7
    :goto_5
    throw v3
    :try_end_e
    .catchall {:try_start_e .. :try_end_e} :catchall_3

    :catchall_3
    move-exception v0

    move-object v3, v0

    :try_start_f
    invoke-virtual {v14}, Ljava/io/FileOutputStream;->close()V
    :try_end_f
    .catchall {:try_start_f .. :try_end_f} :catchall_4

    goto :goto_6

    :catchall_4
    move-exception v0

    move-object v4, v0

    :try_start_10
    invoke-virtual {v3, v4}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_6
    throw v3
    :try_end_10
    .catchall {:try_start_10 .. :try_end_10} :catchall_5

    :catchall_5
    move-exception v0

    :goto_7
    move-object v3, v0

    if-eqz v12, :cond_8

    .line 99
    :try_start_11
    invoke-virtual {v12}, Ljava/nio/channels/FileChannel;->close()V
    :try_end_11
    .catchall {:try_start_11 .. :try_end_11} :catchall_6

    goto :goto_8

    :catchall_6
    move-exception v0

    move-object v4, v0

    :try_start_12
    invoke-virtual {v3, v4}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_8
    :goto_8
    throw v3
    :try_end_12
    .catchall {:try_start_12 .. :try_end_12} :catchall_7

    :catchall_7
    move-exception v0

    move-object v3, v0

    :try_start_13
    invoke-virtual {v11}, Ljava/io/FileInputStream;->close()V
    :try_end_13
    .catchall {:try_start_13 .. :try_end_13} :catchall_8

    goto :goto_9

    :catchall_8
    move-exception v0

    move-object v4, v0

    :try_start_14
    invoke-virtual {v3, v4}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_9
    throw v3
    :try_end_14
    .catchall {:try_start_14 .. :try_end_14} :catchall_9

    :catchall_9
    move-exception v0

    move-object v3, v0

    if-eqz v10, :cond_9

    :try_start_15
    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->close()V
    :try_end_15
    .catchall {:try_start_15 .. :try_end_15} :catchall_a

    goto :goto_a

    :catchall_a
    move-exception v0

    move-object v4, v0

    :try_start_16
    invoke-virtual {v3, v4}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_9
    :goto_a
    throw v3
    :try_end_16
    .catch Ljava/lang/Exception; {:try_start_16 .. :try_end_16} :catch_0

    :catch_0
    move-exception v0

    .line 123
    const-string v3, "BlackImport"

    const-string v4, "import failed"

    invoke-static {v3, v4, v0}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    .line 124
    new-instance v3, Ljava/lang/StringBuilder;

    const-string v4, "\u062a\u0639\u0630\u0631 \u0627\u0644\u0646\u0633\u062e: "

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v0}, Ljava/lang/Exception;->getMessage()Ljava/lang/String;

    move-result-object v0

    invoke-virtual {v3, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    :cond_a
    :goto_b
    if-nez v0, :cond_b

    .line 126
    iget-boolean v3, v2, Lcom/racas/black/GameImporter;->cancelled:Z

    if-nez v3, :cond_b

    move-object/from16 v5, p8

    invoke-virtual {v1, v5}, Ljava/io/File;->renameTo(Ljava/io/File;)Z

    move-result v3

    if-eqz v3, :cond_c

    const/4 v3, 0x1

    goto :goto_c

    :cond_b
    move-object/from16 v5, p8

    :cond_c
    const/4 v3, 0x0

    :goto_c
    move v4, v3

    if-nez v4, :cond_d

    .line 128
    invoke-virtual/range {p3 .. p3}, Ljava/io/File;->delete()Z

    if-nez v0, :cond_d

    .line 129
    iget-boolean v1, v2, Lcom/racas/black/GameImporter;->cancelled:Z

    if-nez v1, :cond_d

    const-string v0, "\u062a\u0639\u0630\u0631 \u0646\u0642\u0644 \u0645\u0644\u0641 \u0627\u0644\u0644\u0639\u0628\u0629 \u0625\u0644\u0649 \u0645\u0643\u0627\u0646\u0647."

    :cond_d
    move-object v8, v0

    .line 132
    iget-object v0, v2, Lcom/racas/black/GameImporter;->main:Landroid/os/Handler;

    new-instance v9, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda5;

    move-object v1, v9

    move-object/from16 v2, p0

    move-object/from16 v3, p9

    move-object/from16 v5, p8

    move-object/from16 v6, p2

    move-object/from16 v7, p10

    invoke-direct/range {v1 .. v8}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda5;-><init>(Lcom/racas/black/GameImporter;Landroid/app/AlertDialog;ZLjava/io/File;Ljava/lang/String;Ljava/util/function/Consumer;Ljava/lang/String;)V

    invoke-virtual {v0, v9}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    return-void
.end method

.method static synthetic lambda$message$8(Ljava/lang/Runnable;Landroid/content/DialogInterface;)V
    .locals 0

    if-eqz p0, :cond_0

    .line 196
    invoke-interface {p0}, Ljava/lang/Runnable;->run()V

    :cond_0
    return-void
.end method

.method private message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V
    .locals 1

    .line 191
    new-instance v0, Landroid/app/AlertDialog$Builder;

    iget-object p0, p0, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-direct {v0, p0}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    .line 192
    invoke-virtual {v0, p1}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p0

    .line 193
    invoke-virtual {p0, p2}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p0

    const-string p1, "\u062d\u0633\u0646\u0627\u064b"

    const/4 p2, 0x0

    .line 194
    invoke-virtual {p0, p1, p2}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object p0

    new-instance p1, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda8;

    invoke-direct {p1, p3}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda8;-><init>(Ljava/lang/Runnable;)V

    .line 195
    invoke-virtual {p0, p1}, Landroid/app/AlertDialog$Builder;->setOnDismissListener(Landroid/content/DialogInterface$OnDismissListener;)Landroid/app/AlertDialog$Builder;

    move-result-object p0

    .line 198
    invoke-virtual {p0}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    return-void
.end method

.method private static show(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V
    .locals 5

    const-wide/16 v0, 0x0

    cmp-long v0, p4, v0

    const-wide v1, 0x41cdcd6500000000L    # 1.0E9

    if-lez v0, :cond_0

    const-wide/16 v3, 0x3e8

    mul-long/2addr v3, p2

    .line 148
    div-long/2addr v3, p4

    long-to-int v0, v3

    invoke-virtual {p0, v0}, Landroid/widget/ProgressBar;->setProgress(I)V

    long-to-double p2, p2

    div-double/2addr p2, v1

    .line 149
    invoke-static {p2, p3}, Ljava/lang/Double;->valueOf(D)Ljava/lang/Double;

    move-result-object p0

    long-to-double p2, p4

    div-double/2addr p2, v1

    invoke-static {p2, p3}, Ljava/lang/Double;->valueOf(D)Ljava/lang/Double;

    move-result-object p2

    filled-new-array {p0, p2}, [Ljava/lang/Object;

    move-result-object p0

    const-string p2, "%.2f \u0645\u0646 %.2f \u063a\u064a\u063a\u0627\u0628\u0627\u064a\u062a"

    invoke-static {p2, p0}, Ljava/lang/String;->format(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object p0

    invoke-virtual {p1, p0}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    goto :goto_0

    :cond_0
    long-to-double p2, p2

    div-double/2addr p2, v1

    .line 151
    invoke-static {p2, p3}, Ljava/lang/Double;->valueOf(D)Ljava/lang/Double;

    move-result-object p0

    filled-new-array {p0}, [Ljava/lang/Object;

    move-result-object p0

    const-string p2, "%.2f \u063a\u064a\u063a\u0627\u0628\u0627\u064a\u062a"

    invoke-static {p2, p0}, Ljava/lang/String;->format(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object p0

    invoke-virtual {p1, p0}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    :goto_0
    return-void
.end method

.method private size(Landroid/net/Uri;)J
    .locals 7

    .line 182
    :try_start_0
    iget-object p0, p0, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-virtual {p0}, Landroid/app/Activity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v0

    const/4 p0, 0x1

    new-array v2, p0, [Ljava/lang/String;

    const-string p0, "_size"

    const/4 v6, 0x0

    aput-object p0, v2, v6

    const/4 v4, 0x0

    const/4 v5, 0x0

    const/4 v3, 0x0

    move-object v1, p1

    invoke-virtual/range {v0 .. v5}, Landroid/content/ContentResolver;->query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;

    move-result-object p0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    if-eqz p0, :cond_2

    .line 183
    :try_start_1
    invoke-interface {p0}, Landroid/database/Cursor;->moveToFirst()Z

    move-result p1

    if-eqz p1, :cond_2

    invoke-interface {p0, v6}, Landroid/database/Cursor;->isNull(I)Z

    move-result p1

    if-nez p1, :cond_2

    invoke-interface {p0, v6}, Landroid/database/Cursor;->getLong(I)J

    move-result-wide v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    if-eqz p0, :cond_0

    .line 184
    :try_start_2
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    :cond_0
    return-wide v0

    :catchall_0
    move-exception p1

    if-eqz p0, :cond_1

    .line 182
    :try_start_3
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception p0

    :try_start_4
    invoke-virtual {p1, p0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_1
    :goto_0
    throw p1

    :cond_2
    if-eqz p0, :cond_3

    .line 184
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    :catch_0
    :cond_3
    const-wide/16 p0, -0x1

    return-wide p0
.end method


# virtual methods
.method copy(Landroid/net/Uri;Ljava/io/File;Ljava/util/function/Consumer;)V
    .locals 15
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "(",
            "Landroid/net/Uri;",
            "Ljava/io/File;",
            "Ljava/util/function/Consumer<",
            "Ljava/lang/Boolean;",
            ">;)V"
        }
    .end annotation

    move-object v1, p0

    move-object/from16 v0, p2

    move-object/from16 v11, p3

    .line 62
    invoke-direct/range {p0 .. p1}, Lcom/racas/black/GameImporter;->displayName(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object v3

    .line 63
    invoke-direct/range {p0 .. p1}, Lcom/racas/black/GameImporter;->size(Landroid/net/Uri;)J

    move-result-wide v7

    if-eqz v3, :cond_3

    .line 64
    invoke-virtual {v3}, Ljava/lang/String;->toLowerCase()Ljava/lang/String;

    move-result-object v2

    const-string v4, ".iso"

    invoke-virtual {v2, v4}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result v2

    if-nez v2, :cond_0

    goto/16 :goto_1

    .line 68
    :cond_0
    invoke-virtual/range {p2 .. p2}, Ljava/io/File;->mkdirs()Z

    const-wide/16 v4, 0x0

    cmp-long v2, v7, v4

    if-lez v2, :cond_1

    .line 69
    invoke-virtual/range {p2 .. p2}, Ljava/io/File;->getUsableSpace()J

    move-result-wide v4

    const-wide/32 v9, 0x4000000

    add-long/2addr v9, v7

    cmp-long v4, v4, v9

    if-gez v4, :cond_1

    long-to-double v2, v7

    const-wide v4, 0x41cdcd6500000000L    # 1.0E9

    div-double/2addr v2, v4

    .line 71
    invoke-static {v2, v3}, Ljava/lang/Double;->valueOf(D)Ljava/lang/Double;

    move-result-object v2

    invoke-virtual/range {p2 .. p2}, Ljava/io/File;->getUsableSpace()J

    move-result-wide v6

    long-to-double v6, v6

    div-double/2addr v6, v4

    invoke-static {v6, v7}, Ljava/lang/Double;->valueOf(D)Ljava/lang/Double;

    move-result-object v0

    filled-new-array {v2, v0}, [Ljava/lang/Object;

    move-result-object v0

    .line 70
    const-string v2, "\u064a\u062d\u062a\u0627\u062c \u0645\u0644\u0641 \u0627\u0644\u0644\u0639\u0628\u0629 \u0625\u0644\u0649 %.1f \u063a\u064a\u063a\u0627\u0628\u0627\u064a\u062a\u060c \u0648\u0627\u0644\u0645\u062a\u0627\u062d %.1f \u063a\u064a\u063a\u0627\u0628\u0627\u064a\u062a."

    invoke-static {v2, v0}, Ljava/lang/String;->format(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v0

    new-instance v2, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda1;

    invoke-direct {v2, v11}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda1;-><init>(Ljava/util/function/Consumer;)V

    const-string v3, "\u0627\u0644\u0645\u0633\u0627\u062d\u0629 \u063a\u064a\u0631 \u0643\u0627\u0641\u064a\u0629"

    invoke-direct {p0, v3, v0, v2}, Lcom/racas/black/GameImporter;->message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V

    return-void

    .line 75
    :cond_1
    new-instance v5, Landroid/widget/ProgressBar;

    iget-object v4, v1, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    const/4 v6, 0x0

    const v9, 0x1010078

    invoke-direct {v5, v4, v6, v9}, Landroid/widget/ProgressBar;-><init>(Landroid/content/Context;Landroid/util/AttributeSet;I)V

    const/16 v4, 0x3e8

    .line 76
    invoke-virtual {v5, v4}, Landroid/widget/ProgressBar;->setMax(I)V

    const/4 v4, 0x1

    const/4 v6, 0x0

    if-gtz v2, :cond_2

    move v2, v4

    goto :goto_0

    :cond_2
    move v2, v6

    .line 77
    :goto_0
    invoke-virtual {v5, v2}, Landroid/widget/ProgressBar;->setIndeterminate(Z)V

    .line 78
    new-instance v9, Landroid/widget/TextView;

    iget-object v2, v1, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-direct {v9, v2}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    .line 79
    const-string v2, "\u062c\u0627\u0631\u064d \u0627\u0644\u0628\u062f\u0621\u2026"

    invoke-virtual {v9, v2}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 80
    new-instance v2, Landroid/widget/LinearLayout;

    iget-object v10, v1, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-direct {v2, v10}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    .line 81
    invoke-virtual {v2, v4}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 82
    iget-object v4, v1, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-virtual {v4}, Landroid/app/Activity;->getResources()Landroid/content/res/Resources;

    move-result-object v4

    invoke-virtual {v4}, Landroid/content/res/Resources;->getDisplayMetrics()Landroid/util/DisplayMetrics;

    move-result-object v4

    iget v4, v4, Landroid/util/DisplayMetrics;->density:F

    const/high16 v10, 0x41a00000    # 20.0f

    mul-float/2addr v4, v10

    invoke-static {v4}, Ljava/lang/Math;->round(F)I

    move-result v4

    .line 83
    div-int/lit8 v10, v4, 0x2

    invoke-virtual {v2, v4, v10, v4, v6}, Landroid/widget/LinearLayout;->setPadding(IIII)V

    .line 84
    invoke-virtual {v2, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 85
    invoke-virtual {v2, v9}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 86
    iput-boolean v6, v1, Lcom/racas/black/GameImporter;->cancelled:Z

    .line 87
    new-instance v4, Landroid/app/AlertDialog$Builder;

    iget-object v10, v1, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    invoke-direct {v4, v10}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    new-instance v10, Ljava/lang/StringBuilder;

    const-string v12, "\u062c\u0627\u0631\u064d \u0627\u0633\u062a\u064a\u0631\u0627\u062f "

    invoke-direct {v10, v12}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v10, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v10

    invoke-virtual {v10}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v10

    .line 88
    invoke-virtual {v4, v10}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v4

    .line 89
    invoke-virtual {v4, v2}, Landroid/app/AlertDialog$Builder;->setView(Landroid/view/View;)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    .line 90
    invoke-virtual {v2, v6}, Landroid/app/AlertDialog$Builder;->setCancelable(Z)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    new-instance v4, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda2;

    invoke-direct {v4, p0}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda2;-><init>(Lcom/racas/black/GameImporter;)V

    .line 91
    const-string v6, "\u0625\u0644\u063a\u0627\u0621"

    invoke-virtual {v2, v6, v4}, Landroid/app/AlertDialog$Builder;->setNegativeButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    .line 92
    invoke-virtual {v2}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    move-result-object v10

    .line 93
    invoke-virtual {v10}, Landroid/app/AlertDialog;->getWindow()Landroid/view/Window;

    move-result-object v2

    const/16 v4, 0x80

    invoke-virtual {v2, v4}, Landroid/view/Window;->addFlags(I)V

    .line 95
    new-instance v4, Ljava/io/File;

    new-instance v2, Ljava/lang/StringBuilder;

    invoke-direct {v2}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v6, ".part"

    invoke-virtual {v2, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-direct {v4, v0, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 96
    new-instance v12, Ljava/io/File;

    invoke-direct {v12, v0, v3}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 97
    new-instance v13, Ljava/lang/Thread;

    new-instance v14, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda3;

    move-object v0, v14

    move-object v1, p0

    move-object/from16 v2, p1

    move-object v6, v9

    move-object v9, v12

    move-object/from16 v11, p3

    invoke-direct/range {v0 .. v11}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda3;-><init>(Lcom/racas/black/GameImporter;Landroid/net/Uri;Ljava/lang/String;Ljava/io/File;Landroid/widget/ProgressBar;Landroid/widget/TextView;JLjava/io/File;Landroid/app/AlertDialog;Ljava/util/function/Consumer;)V

    const-string v0, "GameImport"

    invoke-direct {v13, v14, v0}, Ljava/lang/Thread;-><init>(Ljava/lang/Runnable;Ljava/lang/String;)V

    .line 143
    invoke-virtual {v13}, Ljava/lang/Thread;->start()V

    return-void

    .line 65
    :cond_3
    :goto_1
    new-instance v0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda0;

    invoke-direct {v0, v11}, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda0;-><init>(Ljava/util/function/Consumer;)V

    const-string v2, "\u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    const-string v3, "\u0627\u062e\u062a\u0631 \u0645\u0644\u0641 \u0642\u0631\u0635 \u0627\u0644\u0644\u0639\u0628\u0629 \u0627\u0644\u0630\u064a \u064a\u0646\u062a\u0647\u064a \u0628\u0627\u0644\u0627\u0645\u062a\u062f\u0627\u062f .iso."

    invoke-direct {p0, v2, v3, v0}, Lcom/racas/black/GameImporter;->message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V

    return-void
.end method

.method pick()Z
    .locals 3

    .line 48
    new-instance v0, Landroid/content/Intent;

    const-string v1, "android.intent.action.OPEN_DOCUMENT"

    invoke-direct {v0, v1}, Landroid/content/Intent;-><init>(Ljava/lang/String;)V

    .line 49
    const-string v1, "android.intent.category.OPENABLE"

    invoke-virtual {v0, v1}, Landroid/content/Intent;->addCategory(Ljava/lang/String;)Landroid/content/Intent;

    .line 50
    const-string v1, "*/*"

    invoke-virtual {v0, v1}, Landroid/content/Intent;->setType(Ljava/lang/String;)Landroid/content/Intent;

    .line 52
    :try_start_0
    iget-object v1, p0, Lcom/racas/black/GameImporter;->activity:Landroid/app/Activity;

    const v2, 0x8a11

    invoke-virtual {v1, v0, v2}, Landroid/app/Activity;->startActivityForResult(Landroid/content/Intent;I)V
    :try_end_0
    .catch Landroid/content/ActivityNotFoundException; {:try_start_0 .. :try_end_0} :catch_0

    const/4 p0, 0x1

    return p0

    .line 55
    :catch_0
    const-string v0, "\u0644\u0627 \u064a\u0648\u062c\u062f \u062a\u0637\u0628\u064a\u0642 \u0644\u0627\u062e\u062a\u064a\u0627\u0631 \u0627\u0644\u0645\u0644\u0641\u0627\u062a \u0639\u0644\u0649 \u0647\u0630\u0627 \u0627\u0644\u0647\u0627\u062a\u0641."

    const/4 v1, 0x0

    const-string v2, "\u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-direct {p0, v2, v0, v1}, Lcom/racas/black/GameImporter;->message(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Runnable;)V

    const/4 p0, 0x0

    return p0
.end method
