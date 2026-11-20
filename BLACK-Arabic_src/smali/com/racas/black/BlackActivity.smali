.class public Lcom/racas/black/BlackActivity;
.super Lorg/libsdl/app/SDLActivity;
.source "BlackActivity.java"


# static fields
.field private static final ALL_FILES_ACCESS:Z

.field static final SHARED:Ljava/lang/String; = "Black"

.field private static final STORAGE_REQUEST:I = 0x426c


# instance fields
.field private checking:Z

.field private dialog:Landroid/app/AlertDialog;

.field private final importer:Lcom/racas/black/GameImporter;

.field private importing:Z

.field private notNow:Z

.field private resumed:Z

.field private storageAsked:Z

.field private update:Lcom/racas/black/UpdateChecker$Release;


# direct methods
.method public static synthetic $r8$lambda$7wkk1051M821Q8Jzl7U7jwuxnlU(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$askAllFilesAccess$5(Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$EpoRAQAd7T_2w-1GOIPvFmgkyiI(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;)V
    .locals 0

    invoke-direct {p0, p1}, Lcom/racas/black/BlackActivity;->lambda$askAllFilesAccess$8(Landroid/content/DialogInterface;)V

    return-void
.end method

.method public static synthetic $r8$lambda$HcUGsY3e743zUlXb8t9ibpjHWhg(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$askAllFilesAccess$7(Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$Kh9aGhqvzs2qLt18RmdNpETNI3w(Landroid/view/View;Landroid/view/MotionEvent;)Z
    .locals 0

    invoke-static {p0, p1}, Lcom/racas/black/BlackActivity;->onCapturedPointer(Landroid/view/View;Landroid/view/MotionEvent;)Z

    move-result p0

    return p0
.end method

.method public static synthetic $r8$lambda$SLIJYGNx_a3LFmP7Kf3-ricYwLQ(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$tellWhereTheGameGoes$9(Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$TG6FJnz7Q6GgjUoiFXyEoxahEV4(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$askAllFilesAccess$6(Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$YMNBWKptsF1mnXogkBj72oiPul8(Lcom/racas/black/BlackActivity;)V
    .locals 0

    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->lambda$checkForUpdates$2()V

    return-void
.end method

.method public static synthetic $r8$lambda$dcBTZkt_1Klxqa0eu8OLff2rMhg(Lcom/racas/black/BlackActivity;Ljava/lang/Boolean;)V
    .locals 0

    invoke-direct {p0, p1}, Lcom/racas/black/BlackActivity;->lambda$onActivityResult$11(Ljava/lang/Boolean;)V

    return-void
.end method

.method public static synthetic $r8$lambda$l_j9VUhOF1QKN4ozRVlxDY2KmGg(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2, p3}, Lcom/racas/black/BlackActivity;->lambda$tellOfUpdate$3(Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$mTD55prVSYthp7BTRZonTIJGHQ0(Lcom/racas/black/BlackActivity;Landroid/content/DialogInterface;)V
    .locals 0

    invoke-direct {p0, p1}, Lcom/racas/black/BlackActivity;->lambda$tellWhereTheGameGoes$10(Landroid/content/DialogInterface;)V

    return-void
.end method

.method public static synthetic $r8$lambda$nqewx7y9EgNMCpoSsMDHu44FsSk(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$checkForUpdates$1(Lcom/racas/black/UpdateChecker$Release;Z)V

    return-void
.end method

.method public static synthetic $r8$lambda$okZseP0z6f850zEVOyx2-WlMS1I(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V
    .locals 0

    invoke-direct {p0, p1, p2, p3}, Lcom/racas/black/BlackActivity;->lambda$tellOfUpdate$4(Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V

    return-void
.end method

.method public static synthetic $r8$lambda$pGRB1CPIFwqzQ_x2l-QnuiyYjIo(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 0

    invoke-direct {p0, p1, p2}, Lcom/racas/black/BlackActivity;->lambda$onCreate$0(Lcom/racas/black/UpdateChecker$Release;Z)V

    return-void
.end method

.method static constructor <clinit>()V
    .locals 2

    .line 198
    sget v0, Landroid/os/Build$VERSION;->SDK_INT:I

    const/16 v1, 0x1e

    if-lt v0, v1, :cond_0

    const/4 v0, 0x1

    goto :goto_0

    :cond_0
    const/4 v0, 0x0

    :goto_0
    sput-boolean v0, Lcom/racas/black/BlackActivity;->ALL_FILES_ACCESS:Z

    return-void
.end method

.method public constructor <init>()V
    .locals 1

    .line 36
    invoke-direct {p0}, Lorg/libsdl/app/SDLActivity;-><init>()V

    .line 43
    new-instance v0, Lcom/racas/black/GameImporter;

    invoke-direct {v0, p0}, Lcom/racas/black/GameImporter;-><init>(Landroid/app/Activity;)V

    iput-object v0, p0, Lcom/racas/black/BlackActivity;->importer:Lcom/racas/black/GameImporter;

    return-void
.end method

.method private askAllFilesAccess()V
    .locals 3

    .line 291
    new-instance v0, Landroid/app/AlertDialog$Builder;

    invoke-direct {v0, p0}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    const-string v1, "\u0627\u0644\u0648\u0635\u0648\u0644 \u0625\u0644\u0649 \u0627\u0644\u0645\u0644\u0641\u0627\u062a"

    .line 292
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    .line 295
    sget-boolean v1, Lcom/racas/black/BlackActivity;->ALL_FILES_ACCESS:Z

    if-eqz v1, :cond_0

    .line 296
    const-string v1, "\u0641\u0639\u0651\u0644 \u00ab\u0627\u0644\u0633\u0645\u0627\u062d \u0628\u0625\u062f\u0627\u0631\u0629 \u062c\u0645\u064a\u0639 \u0627\u0644\u0645\u0644\u0641\u0627\u062a\u00bb \u0644\u0644\u062a\u0637\u0628\u064a\u0642 \u0641\u064a \u0627\u0644\u0634\u0627\u0634\u0629 \u0627\u0644\u062a\u0627\u0644\u064a\u0629\u060c \u062b\u0645 \u0627\u0631\u062c\u0639."

    goto :goto_0

    .line 297
    :cond_0
    const-string v1, "\u0627\u0633\u0645\u062d \u0644\u0644\u062a\u0637\u0628\u064a\u0642 \u0628\u0627\u0644\u0648\u0635\u0648\u0644 \u0625\u0644\u0649 \u0627\u0644\u0645\u0644\u0641\u0627\u062a \u0641\u064a \u0627\u0644\u0634\u0627\u0634\u0629 \u0627\u0644\u062a\u0627\u0644\u064a\u0629."

    :goto_0
    const-string v2, "\u064a\u0642\u0631\u0623 \u0627\u0644\u062a\u0637\u0628\u064a\u0642 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629 \u0645\u0646 \u0645\u062c\u0644\u062f Black \u0641\u064a \u0630\u0627\u0643\u0631\u0629 \u0647\u0627\u062a\u0641\u0643:\n\n/storage/emulated/0/Black\n\n"

    invoke-virtual {v2, v1}, Ljava/lang/String;->concat(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v1

    .line 293
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    const/4 v1, 0x0

    .line 298
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setCancelable(Z)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda12;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda12;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 299
    const-string v2, "\u0627\u0644\u0633\u0645\u0627\u062d \u0628\u0627\u0644\u0648\u0635\u0648\u0644"

    invoke-virtual {v0, v2, v1}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda1;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda1;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 300
    const-string v2, "\u0644\u064a\u0633 \u0627\u0644\u0622\u0646"

    invoke-virtual {v0, v2, v1}, Landroid/app/AlertDialog$Builder;->setNegativeButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda2;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda2;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 301
    const-string v2, "\u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-virtual {v0, v2, v1}, Landroid/app/AlertDialog$Builder;->setNeutralButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda3;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda3;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 302
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setOnDismissListener(Landroid/content/DialogInterface$OnDismissListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    .line 303
    invoke-virtual {v0}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    move-result-object v0

    iput-object v0, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    return-void
.end method

.method private askForGame()V
    .locals 4

    .line 209
    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->importing:Z

    if-nez v0, :cond_8

    iget-object v0, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    if-eqz v0, :cond_0

    invoke-virtual {v0}, Landroid/app/AlertDialog;->isShowing()Z

    move-result v0

    if-eqz v0, :cond_0

    goto :goto_0

    :cond_0
    const/4 v0, 0x0

    .line 210
    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->getExternalFilesDir(Ljava/lang/String;)Ljava/io/File;

    move-result-object v0

    .line 211
    const-string v1, "assets"

    if-eqz v0, :cond_1

    new-instance v2, Ljava/io/File;

    invoke-direct {v2, v0, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-static {v2}, Lcom/racas/black/BlackActivity;->hasGame(Ljava/io/File;)Z

    move-result v0

    if-eqz v0, :cond_1

    return-void

    .line 212
    :cond_1
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->hasStorageAccess()Z

    move-result v0

    if-nez v0, :cond_3

    .line 213
    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->notNow:Z

    if-nez v0, :cond_2

    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->askAllFilesAccess()V

    :cond_2
    return-void

    .line 216
    :cond_3
    new-instance v0, Ljava/io/File;

    invoke-static {}, Landroid/os/Environment;->getExternalStorageDirectory()Ljava/io/File;

    move-result-object v2

    const-string v3, "Black"

    invoke-direct {v0, v2, v3}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 217
    invoke-virtual {v0}, Ljava/io/File;->mkdirs()Z

    .line 218
    invoke-static {v0}, Lcom/racas/black/BlackActivity;->hasGame(Ljava/io/File;)Z

    move-result v2

    if-nez v2, :cond_8

    new-instance v2, Ljava/io/File;

    invoke-direct {v2, v0, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-static {v2}, Lcom/racas/black/BlackActivity;->hasGame(Ljava/io/File;)Z

    move-result v0

    if-eqz v0, :cond_4

    goto :goto_0

    .line 219
    :cond_4
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->cardFolders()Ljava/util/List;

    move-result-object v0

    invoke-interface {v0}, Ljava/util/List;->iterator()Ljava/util/Iterator;

    move-result-object v0

    :cond_5
    invoke-interface {v0}, Ljava/util/Iterator;->hasNext()Z

    move-result v2

    if-eqz v2, :cond_7

    invoke-interface {v0}, Ljava/util/Iterator;->next()Ljava/lang/Object;

    move-result-object v2

    check-cast v2, Ljava/io/File;

    .line 220
    invoke-static {v2}, Lcom/racas/black/BlackActivity;->hasGame(Ljava/io/File;)Z

    move-result v3

    if-nez v3, :cond_6

    new-instance v3, Ljava/io/File;

    invoke-direct {v3, v2, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-static {v3}, Lcom/racas/black/BlackActivity;->hasGame(Ljava/io/File;)Z

    move-result v2

    if-eqz v2, :cond_5

    :cond_6
    return-void

    .line 221
    :cond_7
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellWhereTheGameGoes()V

    :cond_8
    :goto_0
    return-void
.end method

.method private cardFolders()Ljava/util/List;
    .locals 6
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "()",
            "Ljava/util/List<",
            "Ljava/io/File;",
            ">;"
        }
    .end annotation

    .line 226
    new-instance v0, Ljava/util/ArrayList;

    invoke-direct {v0}, Ljava/util/ArrayList;-><init>()V

    .line 227
    sget-boolean v1, Lcom/racas/black/BlackActivity;->ALL_FILES_ACCESS:Z

    const-string v2, "Black"

    if-nez v1, :cond_3

    const/4 v1, 0x0

    .line 229
    invoke-virtual {p0, v1}, Lcom/racas/black/BlackActivity;->getExternalFilesDirs(Ljava/lang/String;)[Ljava/io/File;

    move-result-object p0

    const/4 v1, 0x1

    .line 230
    :goto_0
    array-length v3, p0

    if-ge v1, v3, :cond_2

    .line 231
    aget-object v3, p0, v1

    const/4 v4, 0x0

    :goto_1
    if-eqz v3, :cond_0

    const/4 v5, 0x4

    if-ge v4, v5, :cond_0

    .line 232
    invoke-virtual {v3}, Ljava/io/File;->getParentFile()Ljava/io/File;

    move-result-object v3

    add-int/lit8 v4, v4, 0x1

    goto :goto_1

    :cond_0
    if-eqz v3, :cond_1

    .line 233
    new-instance v4, Ljava/io/File;

    invoke-direct {v4, v3, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-interface {v0, v4}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    :cond_1
    add-int/lit8 v1, v1, 0x1

    goto :goto_0

    :cond_2
    return-object v0

    .line 237
    :cond_3
    const-class v1, Landroid/os/storage/StorageManager;

    invoke-virtual {p0, v1}, Lcom/racas/black/BlackActivity;->getSystemService(Ljava/lang/Class;)Ljava/lang/Object;

    move-result-object p0

    check-cast p0, Landroid/os/storage/StorageManager;

    if-nez p0, :cond_4

    return-object v0

    .line 239
    :cond_4
    invoke-virtual {p0}, Landroid/os/storage/StorageManager;->getStorageVolumes()Ljava/util/List;

    move-result-object p0

    invoke-interface {p0}, Ljava/util/List;->iterator()Ljava/util/Iterator;

    move-result-object p0

    :cond_5
    :goto_2
    invoke-interface {p0}, Ljava/util/Iterator;->hasNext()Z

    move-result v1

    if-eqz v1, :cond_7

    invoke-interface {p0}, Ljava/util/Iterator;->next()Ljava/lang/Object;

    move-result-object v1

    check-cast v1, Landroid/os/storage/StorageVolume;

    .line 240
    invoke-virtual {v1}, Landroid/os/storage/StorageVolume;->getDirectory()Ljava/io/File;

    move-result-object v3

    .line 241
    invoke-virtual {v1}, Landroid/os/storage/StorageVolume;->isPrimary()Z

    move-result v4

    if-nez v4, :cond_5

    if-eqz v3, :cond_5

    const-string v4, "mounted"

    invoke-virtual {v1}, Landroid/os/storage/StorageVolume;->getState()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v4, v1}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v1

    if-nez v1, :cond_6

    goto :goto_2

    .line 242
    :cond_6
    new-instance v1, Ljava/io/File;

    invoke-direct {v1, v3, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-interface {v0, v1}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    goto :goto_2

    :cond_7
    return-object v0
.end method

.method private static hasGame(Ljava/io/File;)Z
    .locals 6

    .line 282
    new-instance v0, Ljava/io/File;

    const-string v1, "default.xbe"

    invoke-direct {v0, p0, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v0}, Ljava/io/File;->exists()Z

    move-result v0

    const/4 v1, 0x1

    if-eqz v0, :cond_0

    return v1

    .line 283
    :cond_0
    invoke-virtual {p0}, Ljava/io/File;->listFiles()[Ljava/io/File;

    move-result-object p0

    const/4 v0, 0x0

    if-nez p0, :cond_1

    return v0

    .line 285
    :cond_1
    array-length v2, p0

    move v3, v0

    :goto_0
    if-ge v3, v2, :cond_3

    aget-object v4, p0, v3

    .line 286
    invoke-virtual {v4}, Ljava/io/File;->getName()Ljava/lang/String;

    move-result-object v4

    invoke-virtual {v4}, Ljava/lang/String;->toLowerCase()Ljava/lang/String;

    move-result-object v4

    const-string v5, ".iso"

    invoke-virtual {v4, v5}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result v4

    if-eqz v4, :cond_2

    return v1

    :cond_2
    add-int/lit8 v3, v3, 0x1

    goto :goto_0

    :cond_3
    return v0
.end method

.method private hasStorageAccess()Z
    .locals 1

    .line 203
    sget-boolean v0, Lcom/racas/black/BlackActivity;->ALL_FILES_ACCESS:Z

    if-eqz v0, :cond_0

    invoke-static {}, Landroid/os/Environment;->isExternalStorageManager()Z

    move-result p0

    return p0

    .line 204
    :cond_0
    const-string v0, "android.permission.READ_EXTERNAL_STORAGE"

    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->checkSelfPermission(Ljava/lang/String;)I

    move-result v0

    if-nez v0, :cond_1

    const-string v0, "android.permission.WRITE_EXTERNAL_STORAGE"

    .line 205
    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->checkSelfPermission(Ljava/lang/String;)I

    move-result p0

    if-nez p0, :cond_1

    const/4 p0, 0x1

    goto :goto_0

    :cond_1
    const/4 p0, 0x0

    :goto_0
    return p0
.end method

.method private hideSystemBars()V
    .locals 2

    .line 182
    sget v0, Landroid/os/Build$VERSION;->SDK_INT:I

    const/16 v1, 0x1e

    if-ge v0, v1, :cond_0

    .line 184
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getWindow()Landroid/view/Window;

    move-result-object p0

    invoke-virtual {p0}, Landroid/view/Window;->getDecorView()Landroid/view/View;

    move-result-object p0

    const/16 v0, 0x1706

    invoke-virtual {p0, v0}, Landroid/view/View;->setSystemUiVisibility(I)V

    return-void

    .line 190
    :cond_0
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getWindow()Landroid/view/Window;

    move-result-object p0

    invoke-virtual {p0}, Landroid/view/Window;->getInsetsController()Landroid/view/WindowInsetsController;

    move-result-object p0

    if-nez p0, :cond_1

    return-void

    .line 192
    :cond_1
    invoke-static {}, Landroid/view/WindowInsets$Type;->systemBars()I

    move-result v0

    invoke-interface {p0, v0}, Landroid/view/WindowInsetsController;->hide(I)V

    const/4 v0, 0x2

    .line 193
    invoke-interface {p0, v0}, Landroid/view/WindowInsetsController;->setSystemBarsBehavior(I)V

    return-void
.end method

.method private synthetic lambda$askAllFilesAccess$5(Landroid/content/DialogInterface;I)V
    .locals 0

    .line 299
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->openAccessSettings()V

    return-void
.end method

.method private synthetic lambda$askAllFilesAccess$6(Landroid/content/DialogInterface;I)V
    .locals 0

    const/4 p1, 0x1

    .line 300
    iput-boolean p1, p0, Lcom/racas/black/BlackActivity;->notNow:Z

    return-void
.end method

.method private synthetic lambda$askAllFilesAccess$7(Landroid/content/DialogInterface;I)V
    .locals 0

    .line 301
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->startImport()V

    return-void
.end method

.method private synthetic lambda$askAllFilesAccess$8(Landroid/content/DialogInterface;)V
    .locals 0

    .line 302
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void
.end method

.method private synthetic lambda$checkForUpdates$1(Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 1

    const/4 v0, 0x0

    .line 139
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->checking:Z

    if-eqz p1, :cond_0

    .line 141
    iput-object p1, p0, Lcom/racas/black/BlackActivity;->update:Lcom/racas/black/UpdateChecker$Release;

    .line 142
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void

    .line 145
    :cond_0
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->isFinishing()Z

    move-result p1

    if-nez p1, :cond_4

    iget-object p1, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    if-eqz p1, :cond_1

    invoke-virtual {p1}, Landroid/app/AlertDialog;->isShowing()Z

    move-result p1

    if-eqz p1, :cond_1

    goto :goto_2

    :cond_1
    if-eqz p2, :cond_2

    .line 146
    const-string p1, "\u062a\u0639\u0630\u0631 \u0627\u0644\u0627\u062a\u0635\u0627\u0644 \u0628\u0645\u0635\u062f\u0631 \u0627\u0644\u062a\u062d\u062f\u064a\u062b. \u062a\u062d\u0642\u0642 \u0645\u0646 \u0627\u0644\u0625\u0646\u062a\u0631\u0646\u062a \u0648\u062d\u0627\u0648\u0644 \u0645\u062c\u062f\u062f\u0627\u064b."

    goto :goto_0

    .line 147
    :cond_2
    new-instance p1, Ljava/lang/StringBuilder;

    const-string v0, "\u0644\u062f\u064a\u0643 \u0623\u062d\u062f\u062b \u0625\u0635\u062f\u0627\u0631 ("

    invoke-direct {p1, v0}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-static {p0}, Lcom/racas/black/UpdateChecker;->installedVersion(Landroid/content/Context;)Ljava/lang/String;

    move-result-object v0

    invoke-virtual {p1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string v0, ")."

    invoke-virtual {p1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    .line 148
    :goto_0
    new-instance v0, Landroid/app/AlertDialog$Builder;

    invoke-direct {v0, p0}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    if-eqz p2, :cond_3

    .line 149
    const-string p2, "\u062a\u0639\u0630\u0631 \u0627\u0644\u0628\u062d\u062b \u0639\u0646 \u062a\u062d\u062f\u064a\u062b"

    goto :goto_1

    :cond_3
    const-string p2, "\u0644\u0627 \u064a\u0648\u062c\u062f \u062a\u062d\u062f\u064a\u062b"

    :goto_1
    invoke-virtual {v0, p2}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p2

    .line 150
    invoke-virtual {p2, p1}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p1

    const-string p2, "\u062d\u0633\u0646\u0627\u064b"

    const/4 v0, 0x0

    .line 151
    invoke-virtual {p1, p2, v0}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object p1

    .line 152
    invoke-virtual {p1}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    move-result-object p1

    iput-object p1, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    :cond_4
    :goto_2
    return-void
.end method

.method private synthetic lambda$checkForUpdates$2()V
    .locals 3

    .line 135
    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->checking:Z

    if-eqz v0, :cond_0

    return-void

    :cond_0
    const/4 v0, 0x1

    .line 136
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->checking:Z

    .line 137
    const-string v1, "\u062c\u0627\u0631\u064d \u0627\u0644\u0628\u062d\u062b \u0639\u0646 \u062a\u062d\u062f\u064a\u062b\u2026"

    const/4 v2, 0x0

    invoke-static {p0, v1, v2}, Landroid/widget/Toast;->makeText(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;

    move-result-object v1

    invoke-virtual {v1}, Landroid/widget/Toast;->show()V

    .line 138
    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda11;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda11;-><init>(Lcom/racas/black/BlackActivity;)V

    invoke-static {p0, v0, v1}, Lcom/racas/black/UpdateChecker;->check(Landroid/content/Context;ZLcom/racas/black/UpdateChecker$Listener;)V

    return-void
.end method

.method private synthetic lambda$onActivityResult$11(Ljava/lang/Boolean;)V
    .locals 1

    const/4 v0, 0x0

    .line 342
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->importing:Z

    .line 343
    invoke-virtual {p1}, Ljava/lang/Boolean;->booleanValue()Z

    move-result p1

    if-nez p1, :cond_0

    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->askForGame()V

    .line 344
    :cond_0
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void
.end method

.method private synthetic lambda$onCreate$0(Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 0

    if-nez p1, :cond_0

    return-void

    .line 62
    :cond_0
    iput-object p1, p0, Lcom/racas/black/BlackActivity;->update:Lcom/racas/black/UpdateChecker$Release;

    .line 63
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void
.end method

.method private synthetic lambda$tellOfUpdate$3(Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V
    .locals 0

    .line 266
    iget-object p2, p1, Lcom/racas/black/UpdateChecker$Release;->apk:Ljava/lang/String;

    if-eqz p2, :cond_0

    iget-object p1, p1, Lcom/racas/black/UpdateChecker$Release;->apk:Ljava/lang/String;

    goto :goto_0

    :cond_0
    iget-object p1, p1, Lcom/racas/black/UpdateChecker$Release;->page:Ljava/lang/String;

    :goto_0
    invoke-direct {p0, p1}, Lcom/racas/black/BlackActivity;->open(Ljava/lang/String;)V

    return-void
.end method

.method private synthetic lambda$tellOfUpdate$4(Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V
    .locals 0

    .line 267
    iget-object p1, p1, Lcom/racas/black/UpdateChecker$Release;->tag:Ljava/lang/String;

    invoke-static {p0, p1}, Lcom/racas/black/UpdateChecker;->skip(Landroid/content/Context;Ljava/lang/String;)V

    return-void
.end method

.method private synthetic lambda$tellWhereTheGameGoes$10(Landroid/content/DialogInterface;)V
    .locals 0

    .line 317
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void
.end method

.method private synthetic lambda$tellWhereTheGameGoes$9(Landroid/content/DialogInterface;I)V
    .locals 0

    .line 316
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->startImport()V

    return-void
.end method

.method private static loose(Landroid/view/InputEvent;I)Z
    .locals 1

    .line 76
    sget-boolean v0, Lcom/racas/black/BlackActivity;->mBrokenLibraries:Z

    if-nez v0, :cond_0

    invoke-virtual {p0}, Landroid/view/InputEvent;->getSource()I

    move-result v0

    and-int/2addr p1, v0

    if-eqz p1, :cond_0

    .line 77
    invoke-virtual {p0}, Landroid/view/InputEvent;->getDeviceId()I

    move-result p0

    invoke-static {p0}, Lorg/libsdl/app/SDLControllerManager;->isDeviceSDLJoystick(I)Z

    move-result p0

    if-nez p0, :cond_0

    const/4 p0, 0x1

    goto :goto_0

    :cond_0
    const/4 p0, 0x0

    :goto_0
    return p0
.end method

.method private static native nativeLooseAxes(FFFFFFFF)V
.end method

.method private static native nativeLooseKey(IZ)V
.end method

.method private static native nativeLooseRelease()V
.end method

.method private static onCapturedPointer(Landroid/view/View;Landroid/view/MotionEvent;)Z
    .locals 5

    .line 109
    invoke-virtual {p1}, Landroid/view/MotionEvent;->getActionMasked()I

    move-result p0

    const/4 v0, 0x2

    const/4 v1, 0x0

    const/4 v2, 0x1

    if-eq p0, v0, :cond_3

    const/4 v3, 0x7

    if-eq p0, v3, :cond_3

    const/16 v0, 0x8

    if-eq p0, v0, :cond_2

    const/16 v0, 0xb

    const/4 v3, 0x0

    if-eq p0, v0, :cond_1

    const/16 v0, 0xc

    if-eq p0, v0, :cond_0

    return v2

    .line 120
    :cond_0
    invoke-virtual {p1}, Landroid/view/MotionEvent;->getButtonState()I

    move-result p0

    invoke-static {p0, v2, v3, v3, v2}, Lcom/racas/black/BlackActivity;->onNativeMouse(IIFFZ)V

    return v2

    .line 117
    :cond_1
    invoke-virtual {p1}, Landroid/view/MotionEvent;->getButtonState()I

    move-result p0

    invoke-static {p0, v1, v3, v3, v2}, Lcom/racas/black/BlackActivity;->onNativeMouse(IIFFZ)V

    return v2

    :cond_2
    const/16 p0, 0xa

    .line 123
    invoke-virtual {p1, p0}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result p0

    const/16 v3, 0x9

    .line 124
    invoke-virtual {p1, v3}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result p1

    .line 123
    invoke-static {v1, v0, p0, p1, v1}, Lcom/racas/black/BlackActivity;->onNativeMouse(IIFFZ)V

    return v2

    :cond_3
    move p0, v1

    .line 112
    :goto_0
    invoke-virtual {p1}, Landroid/view/MotionEvent;->getHistorySize()I

    move-result v3

    if-ge p0, v3, :cond_4

    .line 113
    invoke-virtual {p1, p0}, Landroid/view/MotionEvent;->getHistoricalX(I)F

    move-result v3

    invoke-virtual {p1, p0}, Landroid/view/MotionEvent;->getHistoricalY(I)F

    move-result v4

    invoke-static {v1, v0, v3, v4, v2}, Lcom/racas/black/BlackActivity;->onNativeMouse(IIFFZ)V

    add-int/lit8 p0, p0, 0x1

    goto :goto_0

    .line 114
    :cond_4
    invoke-virtual {p1}, Landroid/view/MotionEvent;->getX()F

    move-result p0

    invoke-virtual {p1}, Landroid/view/MotionEvent;->getY()F

    move-result p1

    invoke-static {v1, v0, p0, p1, v2}, Lcom/racas/black/BlackActivity;->onNativeMouse(IIFFZ)V

    return v2
.end method

.method private open(Ljava/lang/String;)V
    .locals 2

    .line 274
    :try_start_0
    new-instance v0, Landroid/content/Intent;

    const-string v1, "android.intent.action.VIEW"

    invoke-static {p1}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object p1

    invoke-direct {v0, v1, p1}, Landroid/content/Intent;-><init>(Ljava/lang/String;Landroid/net/Uri;)V

    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->startActivity(Landroid/content/Intent;)V
    :try_end_0
    .catch Landroid/content/ActivityNotFoundException; {:try_start_0 .. :try_end_0} :catch_0

    :catch_0
    return-void
.end method

.method private openAccessSettings()V
    .locals 6

    .line 351
    sget-boolean v0, Lcom/racas/black/BlackActivity;->ALL_FILES_ACCESS:Z

    const-string v1, "android.settings.APPLICATION_DETAILS_SETTINGS"

    const-string v2, "package:"

    if-nez v0, :cond_2

    .line 352
    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->storageAsked:Z

    const-string v3, "android.permission.READ_EXTERNAL_STORAGE"

    if-eqz v0, :cond_1

    invoke-virtual {p0, v3}, Lcom/racas/black/BlackActivity;->shouldShowRequestPermissionRationale(Ljava/lang/String;)Z

    move-result v0

    if-eqz v0, :cond_0

    goto :goto_0

    .line 358
    :cond_0
    new-instance v0, Landroid/content/Intent;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3, v2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    .line 359
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getPackageName()Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-static {v2}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object v2

    invoke-direct {v0, v1, v2}, Landroid/content/Intent;-><init>(Ljava/lang/String;Landroid/net/Uri;)V

    .line 358
    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->startActivity(Landroid/content/Intent;)V

    goto :goto_1

    :cond_1
    :goto_0
    const/4 v0, 0x1

    .line 354
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->storageAsked:Z

    const/4 v1, 0x2

    .line 355
    new-array v1, v1, [Ljava/lang/String;

    const/4 v2, 0x0

    aput-object v3, v1, v2

    const-string v2, "android.permission.WRITE_EXTERNAL_STORAGE"

    aput-object v2, v1, v0

    const/16 v0, 0x426c

    invoke-virtual {p0, v1, v0}, Lcom/racas/black/BlackActivity;->requestPermissions([Ljava/lang/String;I)V

    :goto_1
    return-void

    .line 364
    :cond_2
    :try_start_0
    new-instance v0, Landroid/content/Intent;

    const-string v3, "android.settings.MANAGE_APP_ALL_FILES_ACCESS_PERMISSION"

    new-instance v4, Ljava/lang/StringBuilder;

    invoke-direct {v4, v2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    .line 365
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getPackageName()Ljava/lang/String;

    move-result-object v5

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v4

    invoke-static {v4}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object v4

    invoke-direct {v0, v3, v4}, Landroid/content/Intent;-><init>(Ljava/lang/String;Landroid/net/Uri;)V

    .line 364
    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->startActivity(Landroid/content/Intent;)V
    :try_end_0
    .catch Landroid/content/ActivityNotFoundException; {:try_start_0 .. :try_end_0} :catch_0

    goto :goto_2

    .line 368
    :catch_0
    :try_start_1
    new-instance v0, Landroid/content/Intent;

    const-string v3, "android.settings.MANAGE_ALL_FILES_ACCESS_PERMISSION"

    invoke-direct {v0, v3}, Landroid/content/Intent;-><init>(Ljava/lang/String;)V

    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->startActivity(Landroid/content/Intent;)V
    :try_end_1
    .catch Landroid/content/ActivityNotFoundException; {:try_start_1 .. :try_end_1} :catch_1

    goto :goto_2

    .line 370
    :catch_1
    new-instance v0, Landroid/content/Intent;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3, v2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    .line 371
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getPackageName()Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-static {v2}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object v2

    invoke-direct {v0, v1, v2}, Landroid/content/Intent;-><init>(Ljava/lang/String;Landroid/net/Uri;)V

    .line 370
    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->startActivity(Landroid/content/Intent;)V

    :goto_2
    return-void
.end method

.method private startImport()V
    .locals 1

    .line 324
    iget-object v0, p0, Lcom/racas/black/BlackActivity;->importer:Lcom/racas/black/GameImporter;

    invoke-virtual {v0}, Lcom/racas/black/GameImporter;->pick()Z

    move-result v0

    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->importing:Z

    return-void
.end method

.method private tellOfUpdate()V
    .locals 6

    .line 249
    iget-object v0, p0, Lcom/racas/black/BlackActivity;->update:Lcom/racas/black/UpdateChecker$Release;

    if-eqz v0, :cond_3

    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->importing:Z

    if-nez v0, :cond_3

    iget-boolean v0, p0, Lcom/racas/black/BlackActivity;->resumed:Z

    if-eqz v0, :cond_3

    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->isFinishing()Z

    move-result v0

    if-nez v0, :cond_3

    iget-object v0, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    if-eqz v0, :cond_0

    invoke-virtual {v0}, Landroid/app/AlertDialog;->isShowing()Z

    move-result v0

    if-eqz v0, :cond_0

    goto/16 :goto_1

    .line 250
    :cond_0
    iget-object v0, p0, Lcom/racas/black/BlackActivity;->update:Lcom/racas/black/UpdateChecker$Release;

    const/4 v1, 0x0

    .line 251
    iput-object v1, p0, Lcom/racas/black/BlackActivity;->update:Lcom/racas/black/UpdateChecker$Release;

    .line 254
    :try_start_0
    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getPackageManager()Landroid/content/pm/PackageManager;

    move-result-object v2

    invoke-virtual {p0}, Lcom/racas/black/BlackActivity;->getPackageName()Ljava/lang/String;

    move-result-object v3

    const/4 v4, 0x0

    invoke-virtual {v2, v3, v4}, Landroid/content/pm/PackageManager;->getPackageInfo(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;

    move-result-object v2

    iget-object v2, v2, Landroid/content/pm/PackageInfo;->versionName:Ljava/lang/String;
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    goto :goto_0

    .line 255
    :catch_0
    const-string v2, ""

    .line 258
    :goto_0
    new-instance v3, Ljava/lang/StringBuilder;

    new-instance v4, Ljava/lang/StringBuilder;

    invoke-direct {v4}, Ljava/lang/StringBuilder;-><init>()V

    iget-object v5, v0, Lcom/racas/black/UpdateChecker$Release;->name:Ljava/lang/String;

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    const-string v5, " \u0645\u062a\u0627\u062d"

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v4

    invoke-direct {v3, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    .line 259
    invoke-virtual {v2}, Ljava/lang/String;->isEmpty()Z

    move-result v4

    if-nez v4, :cond_1

    const-string v4, " (\u0625\u0635\u062f\u0627\u0631\u0643 "

    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v4, ")"

    invoke-virtual {v2, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    :cond_1
    const/16 v2, 0x2e

    .line 260
    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(C)Ljava/lang/StringBuilder;

    .line 261
    iget-object v2, v0, Lcom/racas/black/UpdateChecker$Release;->changes:Ljava/lang/String;

    invoke-virtual {v2}, Ljava/lang/String;->isEmpty()Z

    move-result v2

    if-nez v2, :cond_2

    const-string v2, "\n\n\u0627\u0644\u062c\u062f\u064a\u062f \u0641\u064a \u0627\u0644\u0625\u0635\u062f\u0627\u0631:\n"

    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    iget-object v4, v0, Lcom/racas/black/UpdateChecker$Release;->changes:Ljava/lang/String;

    invoke-virtual {v2, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    .line 262
    :cond_2
    const-string v2, "\n\n\u062b\u0628\u0651\u062a\u0647 \u0641\u0648\u0642 \u0627\u0644\u062a\u0637\u0628\u064a\u0642 \u0627\u0644\u062d\u0627\u0644\u064a \u0644\u0644\u0627\u062d\u062a\u0641\u0627\u0638 \u0628\u0627\u0644\u0625\u0639\u062f\u0627\u062f\u0627\u062a \u0648\u0627\u0644\u062d\u0641\u0638."

    invoke-virtual {v3, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    .line 263
    new-instance v2, Landroid/app/AlertDialog$Builder;

    invoke-direct {v2, p0}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    const-string v4, "\u064a\u062a\u0648\u0641\u0631 \u062a\u062d\u062f\u064a\u062b"

    .line 264
    invoke-virtual {v2, v4}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    .line 265
    invoke-virtual {v2, v3}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    new-instance v3, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda8;

    invoke-direct {v3, p0, v0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda8;-><init>(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;)V

    .line 266
    const-string v4, "\u062a\u0646\u0632\u064a\u0644"

    invoke-virtual {v2, v4, v3}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v2

    new-instance v3, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;

    invoke-direct {v3, p0, v0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;-><init>(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;)V

    .line 267
    const-string v0, "\u062a\u062e\u0637\u064a \u0647\u0630\u0627 \u0627\u0644\u0625\u0635\u062f\u0627\u0631"

    invoke-virtual {v2, v0, v3}, Landroid/app/AlertDialog$Builder;->setNeutralButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    const-string v2, "\u0644\u0627\u062d\u0642\u0627\u064b"

    .line 268
    invoke-virtual {v0, v2, v1}, Landroid/app/AlertDialog$Builder;->setNegativeButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    .line 269
    invoke-virtual {v0}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    move-result-object v0

    iput-object v0, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    :cond_3
    :goto_1
    return-void
.end method

.method private tellWhereTheGameGoes()V
    .locals 3

    .line 307
    new-instance v0, Landroid/app/AlertDialog$Builder;

    invoke-direct {v0, p0}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    const-string v1, "\u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629"

    .line 308
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    const-string v1, "\u0636\u0639 \u0645\u0644\u0641 \u0627\u0644\u0644\u0639\u0628\u0629 \u0641\u064a \u0645\u062c\u0644\u062f Black \u0628\u0630\u0627\u0643\u0631\u0629 \u0627\u0644\u0647\u0627\u062a\u0641:\n\n/storage/emulated/0/Black\n\n\u0623\u0648 \u0641\u064a \u0645\u062c\u0644\u062f Black \u062f\u0627\u062e\u0644 \u0628\u0637\u0627\u0642\u0629 \u0627\u0644\u0630\u0627\u0643\u0631\u0629.\n\n\u064a\u0645\u0643\u0646 \u0627\u0633\u062a\u062e\u062f\u0627\u0645 \u0645\u0644\u0641 ISO\u060c \u0623\u0648 default.xbe \u0645\u0639 \u0645\u062c\u0644\u062f\u0627\u062a\u0647. \u0633\u062a\u0628\u062f\u0623 \u0627\u0644\u0644\u0639\u0628\u0629 \u0639\u0646\u062f \u0627\u0644\u0639\u062b\u0648\u0631 \u0639\u0644\u0649 \u0627\u0644\u0645\u0644\u0641\u0627\u062a.\n\n\u064a\u0645\u0643\u0646\u0643 \u0623\u064a\u0636\u0627\u064b \u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0645\u0644\u0641 \u0627\u0644\u0642\u0631\u0635 \u0645\u0646 \u0627\u0644\u062a\u0646\u0632\u064a\u0644\u0627\u062a \u0623\u0648 \u0627\u0644\u062a\u062e\u0632\u064a\u0646 \u0627\u0644\u0633\u062d\u0627\u0628\u064a \u0623\u0648 \u0630\u0627\u0643\u0631\u0629 USB."

    .line 309
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    const-string v1, "\u062d\u0633\u0646\u0627\u064b"

    const/4 v2, 0x0

    .line 315
    invoke-virtual {v0, v1, v2}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda0;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda0;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 316
    const-string v2, "\u0627\u0633\u062a\u064a\u0631\u0627\u062f \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-virtual {v0, v2, v1}, Landroid/app/AlertDialog$Builder;->setNeutralButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda4;

    invoke-direct {v1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda4;-><init>(Lcom/racas/black/BlackActivity;)V

    .line 317
    invoke-virtual {v0, v1}, Landroid/app/AlertDialog$Builder;->setOnDismissListener(Landroid/content/DialogInterface$OnDismissListener;)Landroid/app/AlertDialog$Builder;

    move-result-object v0

    .line 318
    invoke-virtual {v0}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    move-result-object v0

    iput-object v0, p0, Lcom/racas/black/BlackActivity;->dialog:Landroid/app/AlertDialog;

    return-void
.end method


# virtual methods
.method public checkForUpdates()V
    .locals 0

    invoke-static {p0}, Lcom/racas/black/CinemaActivity;->open(Landroid/app/Activity;)V

    return-void
.end method

.method public dispatchGenericMotionEvent(Landroid/view/MotionEvent;)Z
    .locals 8

    const v0, 0x1000010

    .line 93
    invoke-static {p1, v0}, Lcom/racas/black/BlackActivity;->loose(Landroid/view/InputEvent;I)Z

    move-result v0

    if-eqz v0, :cond_0

    invoke-virtual {p1}, Landroid/view/MotionEvent;->getActionMasked()I

    move-result v0

    const/4 v1, 0x2

    if-ne v0, v1, :cond_0

    const/4 p0, 0x0

    .line 95
    invoke-virtual {p1, p0}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v0

    const/4 p0, 0x1

    invoke-virtual {p1, p0}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v1

    const/16 v2, 0xb

    .line 96
    invoke-virtual {p1, v2}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v2

    const/16 v3, 0xe

    invoke-virtual {p1, v3}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v3

    const/16 v4, 0x11

    .line 97
    invoke-virtual {p1, v4}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v4

    const/16 v5, 0x17

    invoke-virtual {p1, v5}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v5

    invoke-static {v4, v5}, Ljava/lang/Math;->max(FF)F

    move-result v4

    const/16 v5, 0x12

    .line 98
    invoke-virtual {p1, v5}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v5

    const/16 v6, 0x16

    invoke-virtual {p1, v6}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v6

    invoke-static {v5, v6}, Ljava/lang/Math;->max(FF)F

    move-result v5

    const/16 v6, 0xf

    .line 99
    invoke-virtual {p1, v6}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v6

    const/16 v7, 0x10

    invoke-virtual {p1, v7}, Landroid/view/MotionEvent;->getAxisValue(I)F

    move-result v7

    .line 95
    invoke-static/range {v0 .. v7}, Lcom/racas/black/BlackActivity;->nativeLooseAxes(FFFFFFFF)V

    return p0

    .line 102
    :cond_0
    invoke-super {p0, p1}, Lorg/libsdl/app/SDLActivity;->dispatchGenericMotionEvent(Landroid/view/MotionEvent;)Z

    move-result p0

    return p0
.end method

.method public dispatchKeyEvent(Landroid/view/KeyEvent;)Z
    .locals 2

    .line 82
    invoke-virtual {p1}, Landroid/view/KeyEvent;->getKeyCode()I

    move-result v0

    invoke-static {v0}, Landroid/view/KeyEvent;->isGamepadButton(I)Z

    move-result v0

    if-nez v0, :cond_0

    invoke-virtual {p1}, Landroid/view/KeyEvent;->getKeyCode()I

    move-result v0

    const/16 v1, 0x13

    if-lt v0, v1, :cond_3

    .line 83
    invoke-virtual {p1}, Landroid/view/KeyEvent;->getKeyCode()I

    move-result v0

    const/16 v1, 0x16

    if-gt v0, v1, :cond_3

    :cond_0
    const v0, 0x1000411

    .line 84
    invoke-static {p1, v0}, Lcom/racas/black/BlackActivity;->loose(Landroid/view/InputEvent;I)Z

    move-result v0

    if-eqz v0, :cond_3

    .line 85
    invoke-virtual {p1}, Landroid/view/KeyEvent;->getRepeatCount()I

    move-result p0

    const/4 v0, 0x1

    if-nez p0, :cond_2

    invoke-virtual {p1}, Landroid/view/KeyEvent;->getKeyCode()I

    move-result p0

    invoke-virtual {p1}, Landroid/view/KeyEvent;->getAction()I

    move-result p1

    if-nez p1, :cond_1

    move p1, v0

    goto :goto_0

    :cond_1
    const/4 p1, 0x0

    :goto_0
    invoke-static {p0, p1}, Lcom/racas/black/BlackActivity;->nativeLooseKey(IZ)V

    :cond_2
    return v0

    .line 88
    :cond_3
    invoke-super {p0, p1}, Lorg/libsdl/app/SDLActivity;->dispatchKeyEvent(Landroid/view/KeyEvent;)Z

    move-result p0

    return p0
.end method

.method protected getLibraries()[Ljava/lang/String;
    .locals 2

    const/4 p0, 0x2

    .line 49
    new-array p0, p0, [Ljava/lang/String;

    const/4 v0, 0x0

    const-string v1, "SDL3"

    aput-object v1, p0, v0

    const/4 v0, 0x1

    const-string v1, "main"

    aput-object v1, p0, v0

    return-object p0
.end method

.method protected onActivityResult(IILandroid/content/Intent;)V
    .locals 1

    const v0, 0x8a11

    if-eq p1, v0, :cond_0

    .line 330
    invoke-super {p0, p1, p2, p3}, Lorg/libsdl/app/SDLActivity;->onActivityResult(IILandroid/content/Intent;)V

    return-void

    :cond_0
    const/4 p1, -0x1

    const/4 v0, 0x0

    if-ne p2, p1, :cond_1

    if-eqz p3, :cond_1

    .line 333
    invoke-virtual {p3}, Landroid/content/Intent;->getData()Landroid/net/Uri;

    move-result-object p1

    goto :goto_0

    :cond_1
    move-object p1, v0

    :goto_0
    if-nez p1, :cond_2

    const/4 p1, 0x0

    .line 335
    iput-boolean p1, p0, Lcom/racas/black/BlackActivity;->importing:Z

    return-void

    .line 338
    :cond_2
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->hasStorageAccess()Z

    move-result p2

    if-eqz p2, :cond_3

    .line 339
    new-instance p2, Ljava/io/File;

    invoke-static {}, Landroid/os/Environment;->getExternalStorageDirectory()Ljava/io/File;

    move-result-object p3

    const-string v0, "Black"

    invoke-direct {p2, p3, v0}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    goto :goto_1

    .line 340
    :cond_3
    new-instance p2, Ljava/io/File;

    invoke-virtual {p0, v0}, Lcom/racas/black/BlackActivity;->getExternalFilesDir(Ljava/lang/String;)Ljava/io/File;

    move-result-object p3

    const-string v0, "assets"

    invoke-direct {p2, p3, v0}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 341
    :goto_1
    iget-object p3, p0, Lcom/racas/black/BlackActivity;->importer:Lcom/racas/black/GameImporter;

    new-instance v0, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda10;

    invoke-direct {v0, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda10;-><init>(Lcom/racas/black/BlackActivity;)V

    invoke-virtual {p3, p1, p2, v0}, Lcom/racas/black/GameImporter;->copy(Landroid/net/Uri;Ljava/io/File;Ljava/util/function/Consumer;)V

    return-void
.end method

.method protected onCreate(Landroid/os/Bundle;)V
    .locals 5

    .line 54
    invoke-super {p0, p1}, Lorg/libsdl/app/SDLActivity;->onCreate(Landroid/os/Bundle;)V

    const/4 p1, 0x2

    .line 55
    new-array v0, p1, [Landroid/view/View;

    sget-object v1, Lcom/racas/black/BlackActivity;->mSurface:Lorg/libsdl/app/SDLSurface;

    const/4 v2, 0x0

    aput-object v1, v0, v2

    const/4 v1, 0x1

    sget-object v3, Lcom/racas/black/BlackActivity;->mLayout:Landroid/view/ViewGroup;

    aput-object v3, v0, v1

    move v1, v2

    :goto_0
    if-ge v1, p1, :cond_1

    aget-object v3, v0, v1

    if-eqz v3, :cond_0

    .line 56
    new-instance v4, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda5;

    invoke-direct {v4}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda5;-><init>()V

    invoke-virtual {v3, v4}, Landroid/view/View;->setOnCapturedPointerListener(Landroid/view/View$OnCapturedPointerListener;)V

    :cond_0
    add-int/lit8 v1, v1, 0x1

    goto :goto_0

    :cond_1
    const/4 p1, 0x0

    .line 57
    invoke-virtual {p0, p1}, Lcom/racas/black/BlackActivity;->getExternalFilesDir(Ljava/lang/String;)Ljava/io/File;

    move-result-object p1

    if-eqz p1, :cond_2

    .line 58
    new-instance v0, Ljava/io/File;

    const-string v1, "assets"

    invoke-direct {v0, p1, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v0}, Ljava/io/File;->mkdirs()Z

    .line 60
    :cond_2
    new-instance p1, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda6;

    invoke-direct {p1, p0}, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda6;-><init>(Lcom/racas/black/BlackActivity;)V

    invoke-static {p0, v2, p1}, Lcom/racas/black/UpdateChecker;->check(Landroid/content/Context;ZLcom/racas/black/UpdateChecker$Listener;)V

    return-void
.end method

.method protected onPause()V
    .locals 1

    const/4 v0, 0x0

    .line 167
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->resumed:Z

    .line 168
    invoke-super {p0}, Lorg/libsdl/app/SDLActivity;->onPause()V

    return-void
.end method

.method protected onResume()V
    .locals 1

    .line 159
    invoke-super {p0}, Lorg/libsdl/app/SDLActivity;->onResume()V

    const/4 v0, 0x1

    .line 160
    iput-boolean v0, p0, Lcom/racas/black/BlackActivity;->resumed:Z

    .line 161
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->askForGame()V

    .line 162
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->tellOfUpdate()V

    return-void
.end method

.method public onWindowFocusChanged(Z)V
    .locals 0

    .line 175
    invoke-super {p0, p1}, Lorg/libsdl/app/SDLActivity;->onWindowFocusChanged(Z)V

    if-eqz p1, :cond_0

    .line 176
    invoke-direct {p0}, Lcom/racas/black/BlackActivity;->hideSystemBars()V

    goto :goto_0

    .line 177
    :cond_0
    sget-boolean p0, Lcom/racas/black/BlackActivity;->mBrokenLibraries:Z

    if-nez p0, :cond_1

    invoke-static {}, Lcom/racas/black/BlackActivity;->nativeLooseRelease()V

    :cond_1
    :goto_0
    return-void
.end method
