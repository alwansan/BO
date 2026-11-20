.class public final synthetic Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Ljava/lang/Runnable;


# instance fields
.field public final synthetic f$0:Z

.field public final synthetic f$1:Landroid/content/SharedPreferences;

.field public final synthetic f$2:Ljava/lang/String;

.field public final synthetic f$3:Landroid/os/Handler;

.field public final synthetic f$4:Lcom/racas/black/UpdateChecker$Listener;


# direct methods
.method public synthetic constructor <init>(ZLandroid/content/SharedPreferences;Ljava/lang/String;Landroid/os/Handler;Lcom/racas/black/UpdateChecker$Listener;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-boolean p1, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$0:Z

    iput-object p2, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$1:Landroid/content/SharedPreferences;

    iput-object p3, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$2:Ljava/lang/String;

    iput-object p4, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$3:Landroid/os/Handler;

    iput-object p5, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$4:Lcom/racas/black/UpdateChecker$Listener;

    return-void
.end method


# virtual methods
.method public final run()V
    .locals 4

    .line 0
    iget-boolean v0, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$0:Z

    iget-object v1, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$1:Landroid/content/SharedPreferences;

    iget-object v2, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$2:Ljava/lang/String;

    iget-object v3, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$3:Landroid/os/Handler;

    iget-object p0, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda0;->f$4:Lcom/racas/black/UpdateChecker$Listener;

    invoke-static {v0, v1, v2, v3, p0}, Lcom/racas/black/UpdateChecker;->lambda$check$1(ZLandroid/content/SharedPreferences;Ljava/lang/String;Landroid/os/Handler;Lcom/racas/black/UpdateChecker$Listener;)V

    return-void
.end method
