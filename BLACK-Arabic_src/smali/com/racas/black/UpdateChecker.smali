.class final Lcom/racas/black/UpdateChecker;
.super Ljava/lang/Object;
.source "UpdateChecker.java"


# annotations
.annotation system Ldalvik/annotation/MemberClasses;
    value = {
        Lcom/racas/black/UpdateChecker$Listener;,
        Lcom/racas/black/UpdateChecker$Release;
    }
.end annotation


# static fields
.field private static final LATEST:Ljava/lang/String; = "https://api.github.com/repos/RanggaCasper/Black-Android/releases/latest"

.field private static final PREFS:Ljava/lang/String; = "updates"

.field static final REPO:Ljava/lang/String; = "RanggaCasper/Black-Android"

.field private static final SKIPPED:Ljava/lang/String; = "skipped_tag"

.field private static final TAG:Ljava/lang/String; = "BlackUpdate"

.field private static final TIMEOUT_MS:I = 0x1f40


# direct methods
.method private constructor <init>()V
    .locals 0

    .line 44
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method

.method private static changes(Ljava/lang/String;)Ljava/lang/String;
    .locals 8

    .line 125
    const-string v0, "## What\'s Changed"

    invoke-virtual {p0, v0}, Ljava/lang/String;->indexOf(Ljava/lang/String;)I

    move-result v0

    .line 126
    const-string v1, ""

    if-gez v0, :cond_0

    return-object v1

    .line 127
    :cond_0
    new-instance v2, Ljava/lang/StringBuilder;

    invoke-direct {v2}, Ljava/lang/StringBuilder;-><init>()V

    .line 129
    invoke-virtual {p0, v0}, Ljava/lang/String;->substring(I)Ljava/lang/String;

    move-result-object p0

    const-string v0, "\r?\n"

    invoke-virtual {p0, v0}, Ljava/lang/String;->split(Ljava/lang/String;)[Ljava/lang/String;

    move-result-object p0

    array-length v0, p0

    const/4 v3, 0x0

    move v4, v3

    :goto_0
    if-ge v3, v0, :cond_4

    aget-object v5, p0, v3

    .line 130
    invoke-virtual {v5}, Ljava/lang/String;->trim()Ljava/lang/String;

    move-result-object v5

    .line 131
    const-string v6, "**Full Changelog**"

    invoke-virtual {v5, v6}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v6

    if-nez v6, :cond_4

    const-string v6, "## "

    invoke-virtual {v5, v6}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v6

    if-eqz v6, :cond_1

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->length()I

    move-result v6

    if-lez v6, :cond_1

    goto :goto_2

    .line 132
    :cond_1
    const-string v6, "* "

    invoke-virtual {v5, v6}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v6

    if-nez v6, :cond_2

    const-string v6, "- "

    invoke-virtual {v5, v6}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v6

    if-nez v6, :cond_2

    goto :goto_1

    :cond_2
    const/4 v6, 0x2

    .line 133
    invoke-virtual {v5, v6}, Ljava/lang/String;->substring(I)Ljava/lang/String;

    move-result-object v5

    const-string v6, " by @\\S+ in https://\\S+$"

    invoke-virtual {v5, v6, v1}, Ljava/lang/String;->replaceAll(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v5

    const-string v6, " in https://\\S+$"

    invoke-virtual {v5, v6, v1}, Ljava/lang/String;->replaceAll(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v5

    add-int/lit8 v4, v4, 0x1

    const/16 v6, 0xa

    if-le v4, v6, :cond_3

    .line 135
    const-string p0, "\u2022 \u2026\n"

    invoke-virtual {v2, p0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    goto :goto_2

    .line 138
    :cond_3
    const-string v7, "\u2022 "

    invoke-virtual {v2, v7}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v7

    invoke-virtual {v7, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    invoke-virtual {v5, v6}, Ljava/lang/StringBuilder;->append(C)Ljava/lang/StringBuilder;

    :goto_1
    add-int/lit8 v3, v3, 0x1

    goto :goto_0

    .line 140
    :cond_4
    :goto_2
    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/String;->trim()Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method static check(Landroid/content/Context;ZLcom/racas/black/UpdateChecker$Listener;)V
    .locals 0

    return-void
.end method

.method static compare(Ljava/lang/String;Ljava/lang/String;)I
    .locals 4

    .line 145
    invoke-static {p0}, Lcom/racas/black/UpdateChecker;->parts(Ljava/lang/String;)[I

    move-result-object p0

    invoke-static {p1}, Lcom/racas/black/UpdateChecker;->parts(Ljava/lang/String;)[I

    move-result-object p1

    const/4 v0, 0x0

    move v1, v0

    .line 146
    :goto_0
    array-length v2, p0

    array-length v3, p1

    invoke-static {v2, v3}, Ljava/lang/Math;->max(II)I

    move-result v2

    if-ge v1, v2, :cond_3

    .line 147
    array-length v2, p0

    if-ge v1, v2, :cond_0

    aget v2, p0, v1

    goto :goto_1

    :cond_0
    move v2, v0

    :goto_1
    array-length v3, p1

    if-ge v1, v3, :cond_1

    aget v3, p1, v1

    goto :goto_2

    :cond_1
    move v3, v0

    :goto_2
    if-eq v2, v3, :cond_2

    .line 148
    invoke-static {v2, v3}, Ljava/lang/Integer;->compare(II)I

    move-result p0

    return p0

    :cond_2
    add-int/lit8 v1, v1, 0x1

    goto :goto_0

    :cond_3
    return v0
.end method

.method private static fetch()Lcom/racas/black/UpdateChecker$Release;
    .locals 10
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/lang/Exception;
        }
    .end annotation

    .line 86
    const-string v0, "name"

    new-instance v1, Ljava/net/URL;

    const-string v2, "https://api.github.com/repos/RanggaCasper/Black-Android/releases/latest"

    invoke-direct {v1, v2}, Ljava/net/URL;-><init>(Ljava/lang/String;)V

    invoke-virtual {v1}, Ljava/net/URL;->openConnection()Ljava/net/URLConnection;

    move-result-object v1

    check-cast v1, Ljava/net/HttpURLConnection;

    const/16 v2, 0x1f40

    .line 87
    invoke-virtual {v1, v2}, Ljava/net/HttpURLConnection;->setConnectTimeout(I)V

    .line 88
    invoke-virtual {v1, v2}, Ljava/net/HttpURLConnection;->setReadTimeout(I)V

    .line 89
    const-string v2, "Accept"

    const-string v3, "application/vnd.github+json"

    invoke-virtual {v1, v2, v3}, Ljava/net/HttpURLConnection;->setRequestProperty(Ljava/lang/String;Ljava/lang/String;)V

    .line 90
    const-string v2, "User-Agent"

    const-string v3, "Black-Android"

    invoke-virtual {v1, v2, v3}, Ljava/net/HttpURLConnection;->setRequestProperty(Ljava/lang/String;Ljava/lang/String;)V

    .line 92
    :try_start_0
    invoke-virtual {v1}, Ljava/net/HttpURLConnection;->getResponseCode()I

    move-result v2
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    const/16 v3, 0xc8

    const/4 v4, 0x0

    if-eq v2, v3, :cond_0

    .line 110
    invoke-virtual {v1}, Ljava/net/HttpURLConnection;->disconnect()V

    return-object v4

    .line 93
    :cond_0
    :try_start_1
    new-instance v2, Lorg/json/JSONObject;

    invoke-virtual {v1}, Ljava/net/HttpURLConnection;->getInputStream()Ljava/io/InputStream;

    move-result-object v3

    invoke-static {v3}, Lcom/racas/black/UpdateChecker;->read(Ljava/io/InputStream;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    .line 94
    new-instance v3, Lcom/racas/black/UpdateChecker$Release;

    invoke-direct {v3}, Lcom/racas/black/UpdateChecker$Release;-><init>()V

    .line 95
    const-string v5, "tag_name"

    invoke-virtual {v2, v5}, Lorg/json/JSONObject;->getString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v5

    iput-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    .line 96
    iget-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    invoke-virtual {v2, v0, v5}, Lorg/json/JSONObject;->optString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v5

    iput-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->name:Ljava/lang/String;

    .line 97
    iget-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->name:Ljava/lang/String;

    invoke-virtual {v5}, Ljava/lang/String;->isEmpty()Z

    move-result v5

    if-eqz v5, :cond_1

    iget-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    iput-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->name:Ljava/lang/String;

    .line 98
    :cond_1
    const-string v5, "html_url"

    const-string v6, "https://github.com/RanggaCasper/Black-Android/releases/latest"

    invoke-virtual {v2, v5, v6}, Lorg/json/JSONObject;->optString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v5

    iput-object v5, v3, Lcom/racas/black/UpdateChecker$Release;->page:Ljava/lang/String;

    .line 99
    const-string v5, "assets"

    invoke-virtual {v2, v5}, Lorg/json/JSONObject;->optJSONArray(Ljava/lang/String;)Lorg/json/JSONArray;

    move-result-object v5

    const/4 v6, 0x0

    :goto_0
    if-eqz v5, :cond_3

    .line 100
    invoke-virtual {v5}, Lorg/json/JSONArray;->length()I

    move-result v7

    if-ge v6, v7, :cond_3

    .line 101
    invoke-virtual {v5, v6}, Lorg/json/JSONArray;->getJSONObject(I)Lorg/json/JSONObject;

    move-result-object v7

    .line 102
    invoke-virtual {v7, v0}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v8

    invoke-virtual {v8}, Ljava/lang/String;->toLowerCase()Ljava/lang/String;

    move-result-object v8

    const-string v9, ".apk"

    invoke-virtual {v8, v9}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result v8

    if-eqz v8, :cond_2

    .line 103
    const-string v0, "browser_download_url"

    invoke-virtual {v7, v0, v4}, Lorg/json/JSONObject;->optString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0

    iput-object v0, v3, Lcom/racas/black/UpdateChecker$Release;->apk:Ljava/lang/String;

    goto :goto_1

    :cond_2
    add-int/lit8 v6, v6, 0x1

    goto :goto_0

    .line 107
    :cond_3
    :goto_1
    const-string v0, "body"

    const-string v4, ""

    invoke-virtual {v2, v0, v4}, Lorg/json/JSONObject;->optString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0

    invoke-static {v0}, Lcom/racas/black/UpdateChecker;->changes(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0

    iput-object v0, v3, Lcom/racas/black/UpdateChecker$Release;->changes:Ljava/lang/String;
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    .line 110
    invoke-virtual {v1}, Ljava/net/HttpURLConnection;->disconnect()V

    return-object v3

    :catchall_0
    move-exception v0

    invoke-virtual {v1}, Ljava/net/HttpURLConnection;->disconnect()V

    .line 111
    throw v0
.end method

.method static installedVersion(Landroid/content/Context;)Ljava/lang/String;
    .locals 2

    .line 79
    :try_start_0
    invoke-virtual {p0}, Landroid/content/Context;->getPackageManager()Landroid/content/pm/PackageManager;

    move-result-object v0

    invoke-virtual {p0}, Landroid/content/Context;->getPackageName()Ljava/lang/String;

    move-result-object p0

    const/4 v1, 0x0

    invoke-virtual {v0, p0, v1}, Landroid/content/pm/PackageManager;->getPackageInfo(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;

    move-result-object p0

    iget-object p0, p0, Landroid/content/pm/PackageInfo;->versionName:Ljava/lang/String;
    :try_end_0
    .catch Landroid/content/pm/PackageManager$NameNotFoundException; {:try_start_0 .. :try_end_0} :catch_0

    return-object p0

    .line 81
    :catch_0
    const-string p0, "0"

    return-object p0
.end method

.method static synthetic lambda$check$0(Lcom/racas/black/UpdateChecker$Listener;Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 0

    .line 68
    invoke-interface {p0, p1, p2}, Lcom/racas/black/UpdateChecker$Listener;->onResult(Lcom/racas/black/UpdateChecker$Release;Z)V

    return-void
.end method

.method static synthetic lambda$check$1(ZLandroid/content/SharedPreferences;Ljava/lang/String;Landroid/os/Handler;Lcom/racas/black/UpdateChecker$Listener;)V
    .locals 7

    .line 53
    const-string v0, "BlackUpdate"

    .line 0
    const-string v1, "update "

    const/4 v2, 0x1

    const/4 v3, 0x0

    .line 56
    :try_start_0
    invoke-static {}, Lcom/racas/black/UpdateChecker;->fetch()Lcom/racas/black/UpdateChecker$Release;

    move-result-object v4

    const/4 v5, 0x0

    if-eqz v4, :cond_0

    if-nez p0, :cond_0

    .line 57
    iget-object p0, v4, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    const-string v6, "skipped_tag"

    invoke-interface {p1, v6, v3}, Landroid/content/SharedPreferences;->getString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object p1

    invoke-virtual {p0, p1}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result p0

    if-eqz p0, :cond_0

    move p0, v2

    goto :goto_0

    :cond_0
    move p0, v5

    :goto_0
    if-eqz v4, :cond_1

    if-nez p0, :cond_1

    .line 58
    iget-object p0, v4, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    invoke-static {p0, p2}, Lcom/racas/black/UpdateChecker;->compare(Ljava/lang/String;Ljava/lang/String;)I

    move-result p0

    if-lez p0, :cond_1

    .line 59
    new-instance p0, Ljava/lang/StringBuilder;

    invoke-direct {p0, v1}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    iget-object p1, v4, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    invoke-virtual {p0, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    const-string p1, " over "

    invoke-virtual {p0, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    invoke-static {v0, p0}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    move-object v3, v4

    :cond_1
    move v2, v5

    goto :goto_1

    :catch_0
    move-exception p0

    .line 63
    new-instance p1, Ljava/lang/StringBuilder;

    const-string p2, "no update check: "

    invoke-direct {p1, p2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {p1, p0}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    invoke-static {v0, p0}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;)I

    .line 68
    :goto_1
    new-instance p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;

    invoke-direct {p0, p4, v3, v2}, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;-><init>(Lcom/racas/black/UpdateChecker$Listener;Lcom/racas/black/UpdateChecker$Release;Z)V

    invoke-virtual {p3, p0}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    return-void
.end method

.method private static parts(Ljava/lang/String;)[I
    .locals 6

    const/4 v0, 0x0

    if-nez p0, :cond_0

    .line 154
    new-array p0, v0, [I

    return-object p0

    .line 155
    :cond_0
    invoke-virtual {p0}, Ljava/lang/String;->trim()Ljava/lang/String;

    move-result-object p0

    .line 156
    const-string v1, "v"

    invoke-virtual {p0, v1}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v1

    if-nez v1, :cond_1

    const-string v1, "V"

    invoke-virtual {p0, v1}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v1

    if-eqz v1, :cond_2

    :cond_1
    const/4 v1, 0x1

    invoke-virtual {p0, v1}, Ljava/lang/String;->substring(I)Ljava/lang/String;

    move-result-object p0

    :cond_2
    const/16 v1, 0x2d

    .line 157
    invoke-virtual {p0, v1}, Ljava/lang/String;->indexOf(I)I

    move-result v1

    if-ltz v1, :cond_3

    .line 158
    invoke-virtual {p0, v0, v1}, Ljava/lang/String;->substring(II)Ljava/lang/String;

    move-result-object p0

    .line 159
    :cond_3
    const-string v1, "\\."

    invoke-virtual {p0, v1}, Ljava/lang/String;->split(Ljava/lang/String;)[Ljava/lang/String;

    move-result-object p0

    .line 160
    array-length v1, p0

    new-array v1, v1, [I

    move v2, v0

    .line 161
    :goto_0
    array-length v3, p0

    if-ge v2, v3, :cond_4

    .line 163
    :try_start_0
    aget-object v3, p0, v2

    const-string v4, "\\D.*"

    const-string v5, ""

    invoke-virtual {v3, v4, v5}, Ljava/lang/String;->replaceAll(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-static {v3}, Ljava/lang/Integer;->parseInt(Ljava/lang/String;)I

    move-result v3

    aput v3, v1, v2
    :try_end_0
    .catch Ljava/lang/NumberFormatException; {:try_start_0 .. :try_end_0} :catch_0

    goto :goto_1

    .line 165
    :catch_0
    aput v0, v1, v2

    :goto_1
    add-int/lit8 v2, v2, 0x1

    goto :goto_0

    :cond_4
    return-object v1
.end method

.method private static read(Ljava/io/InputStream;)Ljava/lang/String;
    .locals 4
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/lang/Exception;
        }
    .end annotation

    .line 116
    :try_start_0
    new-instance v0, Ljava/io/ByteArrayOutputStream;

    invoke-direct {v0}, Ljava/io/ByteArrayOutputStream;-><init>()V

    const/16 v1, 0x2000

    .line 117
    new-array v1, v1, [B

    .line 118
    :goto_0
    invoke-virtual {p0, v1}, Ljava/io/InputStream;->read([B)I

    move-result v2

    if-lez v2, :cond_0

    const/4 v3, 0x0

    invoke-virtual {v0, v1, v3, v2}, Ljava/io/ByteArrayOutputStream;->write([BII)V

    goto :goto_0

    .line 119
    :cond_0
    sget-object v1, Ljava/nio/charset/StandardCharsets;->UTF_8:Ljava/nio/charset/Charset;

    invoke-virtual {v1}, Ljava/nio/charset/Charset;->name()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v0, v1}, Ljava/io/ByteArrayOutputStream;->toString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    if-eqz p0, :cond_1

    .line 120
    invoke-virtual {p0}, Ljava/io/InputStream;->close()V

    :cond_1
    return-object v0

    :catchall_0
    move-exception v0

    if-eqz p0, :cond_2

    .line 115
    :try_start_1
    invoke-virtual {p0}, Ljava/io/InputStream;->close()V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception p0

    invoke-virtual {v0, p0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_2
    :goto_1
    throw v0
.end method

.method static skip(Landroid/content/Context;Ljava/lang/String;)V
    .locals 2

    .line 73
    invoke-virtual {p0}, Landroid/content/Context;->getApplicationContext()Landroid/content/Context;

    move-result-object p0

    const-string v0, "updates"

    const/4 v1, 0x0

    invoke-virtual {p0, v0, v1}, Landroid/content/Context;->getSharedPreferences(Ljava/lang/String;I)Landroid/content/SharedPreferences;

    move-result-object p0

    .line 74
    invoke-interface {p0}, Landroid/content/SharedPreferences;->edit()Landroid/content/SharedPreferences$Editor;

    move-result-object p0

    const-string v0, "skipped_tag"

    invoke-interface {p0, v0, p1}, Landroid/content/SharedPreferences$Editor;->putString(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;

    move-result-object p0

    invoke-interface {p0}, Landroid/content/SharedPreferences$Editor;->apply()V

    return-void
.end method
