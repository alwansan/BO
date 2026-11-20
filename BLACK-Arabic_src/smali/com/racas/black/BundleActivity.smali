.class public final Lcom/racas/black/BundleActivity;
.super Landroid/app/Activity;
.source "BundleActivity.java"


# static fields
.field private static final SHA:Ljava/lang/String; = "f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62"

.field private static final SIZE:J = 0x6ae1c000L


# instance fields
.field private volatile closed:Z

.field private lastUi:J

.field private final main:Landroid/os/Handler;

.field private message:Landroid/widget/TextView;

.field private progress:Landroid/widget/ProgressBar;

.field private ready:Z

.field private resumed:Z

.field private working:Z


# direct methods
.method public constructor <init>()V
    .locals 1

    .line 15
    invoke-direct {p0}, Landroid/app/Activity;-><init>()V

    .line 16
    new-instance v0, Landroid/os/Handler;

    invoke-direct {v0}, Landroid/os/Handler;-><init>()V

    iput-object v0, p0, Lcom/racas/black/BundleActivity;->main:Landroid/os/Handler;

    return-void
.end method

.method static synthetic access$000(Lcom/racas/black/BundleActivity;)Z
    .locals 0

    .line 15
    iget-boolean p0, p0, Lcom/racas/black/BundleActivity;->closed:Z

    return p0
.end method

.method static synthetic access$100(Lcom/racas/black/BundleActivity;)J
    .locals 2

    .line 15
    iget-wide v0, p0, Lcom/racas/black/BundleActivity;->lastUi:J

    return-wide v0
.end method

.method static synthetic access$102(Lcom/racas/black/BundleActivity;J)J
    .locals 0

    .line 15
    iput-wide p1, p0, Lcom/racas/black/BundleActivity;->lastUi:J

    return-wide p1
.end method

.method static synthetic access$200(Lcom/racas/black/BundleActivity;)Landroid/os/Handler;
    .locals 0

    .line 15
    iget-object p0, p0, Lcom/racas/black/BundleActivity;->main:Landroid/os/Handler;

    return-object p0
.end method

.method static synthetic access$300(Lcom/racas/black/BundleActivity;)Landroid/widget/ProgressBar;
    .locals 0

    .line 15
    iget-object p0, p0, Lcom/racas/black/BundleActivity;->progress:Landroid/widget/ProgressBar;

    return-object p0
.end method

.method static synthetic access$400(Lcom/racas/black/BundleActivity;)Landroid/widget/TextView;
    .locals 0

    .line 15
    iget-object p0, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    return-object p0
.end method

.method static synthetic access$502(Lcom/racas/black/BundleActivity;Z)Z
    .locals 0

    .line 15
    iput-boolean p1, p0, Lcom/racas/black/BundleActivity;->working:Z

    return p1
.end method

.method static synthetic access$602(Lcom/racas/black/BundleActivity;Z)Z
    .locals 0

    .line 15
    iput-boolean p1, p0, Lcom/racas/black/BundleActivity;->ready:Z

    return p1
.end method

.method static synthetic access$700(Lcom/racas/black/BundleActivity;)V
    .locals 0

    .line 15
    invoke-direct {p0}, Lcom/racas/black/BundleActivity;->launchIfReady()V

    return-void
.end method

.method private begin(Landroid/widget/Button;)V
    .locals 2

    .line 39
    iget-boolean v0, p0, Lcom/racas/black/BundleActivity;->working:Z

    if-eqz v0, :cond_0

    return-void

    :cond_0
    const/4 v0, 0x1

    iput-boolean v0, p0, Lcom/racas/black/BundleActivity;->working:Z

    iget-object v0, p0, Lcom/racas/black/BundleActivity;->progress:Landroid/widget/ProgressBar;

    const/4 v1, 0x0

    invoke-virtual {v0, v1}, Landroid/widget/ProgressBar;->setVisibility(I)V

    iget-object v0, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    const-string v1, "\u062c\u0627\u0631\u064d \u0627\u0644\u062a\u062d\u0642\u0642 \u0645\u0646 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u0644\u0639\u0628\u0629\u2026"

    invoke-virtual {v0, v1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 40
    new-instance v0, Ljava/lang/Thread;

    new-instance v1, Lcom/racas/black/BundleActivity$1;

    invoke-direct {v1, p0, p1}, Lcom/racas/black/BundleActivity$1;-><init>(Lcom/racas/black/BundleActivity;Landroid/widget/Button;)V

    const-string p1, "BLACK-bundle-prepare"

    invoke-direct {v0, v1, p1}, Ljava/lang/Thread;-><init>(Ljava/lang/Runnable;Ljava/lang/String;)V

    .line 53
    invoke-virtual {v0}, Ljava/lang/Thread;->start()V

    .line 54
    return-void
.end method

.method private launchIfReady()V
    .locals 2

    .line 55
    iget-boolean v0, p0, Lcom/racas/black/BundleActivity;->ready:Z

    if-eqz v0, :cond_0

    iget-boolean v0, p0, Lcom/racas/black/BundleActivity;->resumed:Z

    if-eqz v0, :cond_0

    iget-boolean v0, p0, Lcom/racas/black/BundleActivity;->closed:Z

    if-nez v0, :cond_0

    const/4 v0, 0x0

    iput-boolean v0, p0, Lcom/racas/black/BundleActivity;->ready:Z

    new-instance v0, Landroid/content/Intent;

    invoke-direct {v0}, Landroid/content/Intent;-><init>()V

    const-string v1, "com.racas.black.BlackActivity"

    invoke-virtual {v0, p0, v1}, Landroid/content/Intent;->setClassName(Landroid/content/Context;Ljava/lang/String;)Landroid/content/Intent;

    move-result-object v0

    invoke-virtual {p0, v0}, Lcom/racas/black/BundleActivity;->startActivity(Landroid/content/Intent;)V

    invoke-virtual {p0}, Lcom/racas/black/BundleActivity;->finish()V

    :cond_0
    return-void
.end method


# virtual methods
.method synthetic lambda$onCreate$0$com-racas-black-BundleActivity(Landroid/widget/Button;Landroid/view/View;)V
    .locals 0

    .line 34
    const/16 p2, 0x8

    invoke-virtual {p1, p2}, Landroid/widget/Button;->setVisibility(I)V

    invoke-direct {p0, p1}, Lcom/racas/black/BundleActivity;->begin(Landroid/widget/Button;)V

    return-void
.end method

.method public onCreate(Landroid/os/Bundle;)V
    .locals 9

    .line 25
    invoke-super {p0, p1}, Landroid/app/Activity;->onCreate(Landroid/os/Bundle;)V

    .line 26
    invoke-virtual {p0}, Lcom/racas/black/BundleActivity;->getWindow()Landroid/view/Window;

    move-result-object p1

    const/16 v0, 0x400

    invoke-virtual {p1, v0, v0}, Landroid/view/Window;->setFlags(II)V

    invoke-virtual {p0}, Lcom/racas/black/BundleActivity;->getWindow()Landroid/view/Window;

    move-result-object p1

    invoke-virtual {p1}, Landroid/view/Window;->getDecorView()Landroid/view/View;

    move-result-object p1

    const/16 v0, 0x1706

    invoke-virtual {p1, v0}, Landroid/view/View;->setSystemUiVisibility(I)V

    .line 27
    invoke-virtual {p0}, Lcom/racas/black/BundleActivity;->getWindow()Landroid/view/Window;

    move-result-object p1

    const/16 v0, 0x80

    invoke-virtual {p1, v0}, Landroid/view/Window;->addFlags(I)V

    .line 28
    new-instance p1, Landroid/widget/LinearLayout;

    invoke-direct {p1, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    const/4 v0, 0x1

    invoke-virtual {p1, v0}, Landroid/widget/LinearLayout;->setOrientation(I)V

    const/16 v0, 0x11

    invoke-virtual {p1, v0}, Landroid/widget/LinearLayout;->setGravity(I)V

    const/16 v1, 0x28

    const/16 v2, 0x1e

    invoke-virtual {p1, v1, v2, v1, v2}, Landroid/widget/LinearLayout;->setPadding(IIII)V

    const/16 v1, 0x10

    const/16 v3, 0xe

    const/16 v4, 0x8

    invoke-static {v4, v1, v3}, Landroid/graphics/Color;->rgb(III)I

    move-result v1

    invoke-virtual {p1, v1}, Landroid/widget/LinearLayout;->setBackgroundColor(I)V

    .line 29
    new-instance v1, Landroid/widget/TextView;

    invoke-direct {v1, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    const-string v3, "BLACK \u0639\u0631\u0628\u064a"

    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    const/high16 v3, 0x42000000    # 32.0f

    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setTextSize(F)V

    invoke-virtual {v1, v0}, Landroid/widget/TextView;->setGravity(I)V

    const/4 v3, -0x1

    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setTextColor(I)V

    invoke-virtual {p1, v1}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 30
    new-instance v5, Landroid/widget/TextView;

    invoke-direct {v5, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    iput-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    const/high16 v6, 0x41980000    # 19.0f

    invoke-virtual {v5, v6}, Landroid/widget/TextView;->setTextSize(F)V

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    invoke-virtual {v5, v0}, Landroid/widget/TextView;->setGravity(I)V

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    invoke-virtual {v5, v3}, Landroid/widget/TextView;->setTextColor(I)V

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    const/16 v6, 0x14

    const/4 v7, 0x0

    invoke-virtual {v5, v7, v6, v7, v6}, Landroid/widget/TextView;->setPadding(IIII)V

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    invoke-virtual {p1, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 31
    new-instance v5, Landroid/widget/ProgressBar;

    const/4 v6, 0x0

    const v8, 0x1010078

    invoke-direct {v5, p0, v6, v8}, Landroid/widget/ProgressBar;-><init>(Landroid/content/Context;Landroid/util/AttributeSet;I)V

    iput-object v5, p0, Lcom/racas/black/BundleActivity;->progress:Landroid/widget/ProgressBar;

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->progress:Landroid/widget/ProgressBar;

    const/16 v6, 0x3e8

    invoke-virtual {v5, v6}, Landroid/widget/ProgressBar;->setMax(I)V

    iget-object v5, p0, Lcom/racas/black/BundleActivity;->progress:Landroid/widget/ProgressBar;

    new-instance v6, Landroid/widget/LinearLayout$LayoutParams;

    invoke-direct {v6, v3, v2}, Landroid/widget/LinearLayout$LayoutParams;-><init>(II)V

    invoke-virtual {p1, v5, v6}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 32
    new-instance v2, Landroid/widget/TextView;

    invoke-direct {v2, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    const-string v3, "\u0627\u0644\u0645\u0639\u0631\u0651\u0628: \u0645\u062d\u0645\u062f \u0639\u062f\u0646\u0627\u0646 (@MohammePS4)"

    invoke-virtual {v2, v3}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    const v3, -0x333334

    invoke-virtual {v2, v3}, Landroid/widget/TextView;->setTextColor(I)V

    invoke-virtual {v2, v0}, Landroid/widget/TextView;->setGravity(I)V

    const/16 v0, 0x18

    invoke-virtual {v2, v7, v0, v7, v7}, Landroid/widget/TextView;->setPadding(IIII)V

    invoke-virtual {p1, v2}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 33
    new-instance v0, Landroid/widget/Button;

    invoke-direct {v0, p0}, Landroid/widget/Button;-><init>(Landroid/content/Context;)V

    const-string v3, "\u0625\u0639\u0627\u062f\u0629 \u0627\u0644\u0645\u062d\u0627\u0648\u0644\u0629"

    invoke-virtual {v0, v3}, Landroid/widget/Button;->setText(Ljava/lang/CharSequence;)V

    invoke-virtual {v0, v4}, Landroid/widget/Button;->setVisibility(I)V

    invoke-virtual {p1, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 34
    new-instance v3, Lcom/racas/black/BundleActivity$$ExternalSyntheticLambda0;

    invoke-direct {v3, p0, v0}, Lcom/racas/black/BundleActivity$$ExternalSyntheticLambda0;-><init>(Lcom/racas/black/BundleActivity;Landroid/widget/Button;)V

    invoke-virtual {v0, v3}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 35
    :try_start_0
    invoke-virtual {p0}, Lcom/racas/black/BundleActivity;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v3

    const-string v4, "thmanyah-Medium.otf"

    invoke-static {v3, v4}, Landroid/graphics/Typeface;->createFromAsset(Landroid/content/res/AssetManager;Ljava/lang/String;)Landroid/graphics/Typeface;

    move-result-object v3

    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    iget-object v1, p0, Lcom/racas/black/BundleActivity;->message:Landroid/widget/TextView;

    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    invoke-virtual {v2, v3}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    invoke-virtual {v0, v3}, Landroid/widget/Button;->setTypeface(Landroid/graphics/Typeface;)V
    :try_end_0
    .catch Ljava/lang/RuntimeException; {:try_start_0 .. :try_end_0} :catch_0

    goto :goto_0

    :catch_0
    move-exception v1

    .line 36
    :goto_0
    invoke-virtual {p0, p1}, Lcom/racas/black/BundleActivity;->setContentView(Landroid/view/View;)V

    invoke-direct {p0, v0}, Lcom/racas/black/BundleActivity;->begin(Landroid/widget/Button;)V

    .line 37
    return-void
.end method

.method protected onDestroy()V
    .locals 2

    .line 58
    const/4 v0, 0x1

    iput-boolean v0, p0, Lcom/racas/black/BundleActivity;->closed:Z

    iget-object v0, p0, Lcom/racas/black/BundleActivity;->main:Landroid/os/Handler;

    const/4 v1, 0x0

    invoke-virtual {v0, v1}, Landroid/os/Handler;->removeCallbacksAndMessages(Ljava/lang/Object;)V

    invoke-super {p0}, Landroid/app/Activity;->onDestroy()V

    return-void
.end method

.method protected onPause()V
    .locals 1

    .line 57
    const/4 v0, 0x0

    iput-boolean v0, p0, Lcom/racas/black/BundleActivity;->resumed:Z

    invoke-super {p0}, Landroid/app/Activity;->onPause()V

    return-void
.end method

.method protected onResume()V
    .locals 1

    .line 56
    invoke-super {p0}, Landroid/app/Activity;->onResume()V

    const/4 v0, 0x1

    iput-boolean v0, p0, Lcom/racas/black/BundleActivity;->resumed:Z

    invoke-direct {p0}, Lcom/racas/black/BundleActivity;->launchIfReady()V

    return-void
.end method
